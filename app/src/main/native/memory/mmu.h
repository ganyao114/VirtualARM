//
// Created by 甘尧 on 2020-03-04.
//

#pragma once

#include <base/marcos.h>
#include <vector>
#include "tlb.h"

namespace Memory {

    template<typename AddrType, u8 page_bits>
    class PageTableWithHostPageAlign {
    public:

        struct PageAddr : AddrType {
            AddrType page_start:sizeof(AddrType) * 8 - page_bits;
            AddrType page_attrs:page_bits;
        };

        PageTableWithHostPageAlign(u8 addr_width) : addr_width_(addr_width) {
            const std::size_t num_page_table_entries = 1ULL
                    << (addr_width_ - page_bits);
            pages_.resize(addr_width);
            pages_.shrink_to_fit();
        }

        VAddr TableAddr() {
            return pages_.data();
        }

    protected:
        u8 addr_width_;
        std::vector<PageAddr> pages_;
    };

    template<typename AddrType, typename AttrType>
    class FlatPageTable : public BaseObject {
    public:

        FlatPageTable(u8 page_bits, u8 addr_width) : page_bits_(page_bits),
                                                     addr_width_(addr_width) {
            const std::size_t num_page_table_entries = 1ULL
                    << (addr_width_ - page_bits);
            pages_.resize(num_page_table_entries);
            pages_.shrink_to_fit();
            attrs_.resize(num_page_table_entries);
            attrs_.shrink_to_fit();
        }

        const std::vector<AddrType> &GetPages() const {
            return pages_;
        }

        const std::vector<AttrType> &GetAttrs() const {
            return attrs_;
        }

    protected:
        const u8 addr_width_;
        const u8 page_bits_;
        std::vector<AddrType> pages_;
        std::vector<AttrType> attrs_;
    };

    template<typename AddrType, typename PTE>
    class MMU : public BaseObject {
    public:

        using Table = VAddr *;
        using FinalTable = PTE *;

        MMU(u8 page_bits, u8 addr_width, bool tlb_per_thread = false) : page_bits_(page_bits)
                                                                    , addr_width_(addr_width)
                                                                    , page_size_((AddrType) 1 << page_bits_)
                                                                    , page_mask_(page_size_ - 1) {
            pte_bits_ = addr_width_ - page_bits_;
            if (pte_bits_ >= 48 && pte_bits_ % 3 == 0) {
                // 3级页表
                level_ = 3;
            } else {
                level_ = 2;
            }
            assert(pte_bits_ % level_ == 0);
            pte_bits_ /= level_;
            pte_size_ = 1ULL << pte_bits_;
            pages_.resize(pte_size_);
            if (!tlb_per_thread) {
                tlb_ = SharedPtr<TLB<AddrType, PTE>>(new TLB<AddrType, PTE>(page_bits_, 16));
            }
        }

        u8 GetPteBits() const {
            return pte_bits_;
        }

        u8 GetPageBits() const {
            return page_bits_;
        }

        u8 GetLevel() const {
            return level_;
        }

        u8 GetAddressWidth() const {
            return addr_width_;
        }

        u8 GetUnusedBits() const {
            return sizeof(AddrType) - addr_width_;
        }

        void MapPage(AddrType vaddr, PTE &pte) {
            assert(vaddr % (1 << page_bits_) == 0);
            AddrType all_page_bits = BitRange<AddrType>(vaddr, page_bits_,
                                                        sizeof(AddrType) - addr_width_ - 1);
            AddrType index;
            Table table = reinterpret_cast<Table>(pages_.data());
            for (int i = 0; i < level_; ++i) {
                index = BitRange<AddrType>(all_page_bits, pte_bits_ * (level_ - i - 1),
                                           pte_bits_ * (level_ - i) - 1);
                if (i < level_ - 2) {
                    auto tmp = reinterpret_cast<Table>(table[index]);
                    if (tmp == nullptr) {
                        tmp = static_cast<Table>(malloc(sizeof(Table) * pte_size_));
                        table[index] = reinterpret_cast<VAddr>(tmp);
                    }
                    table = tmp;
                } else {
                    //Final level
                    FinalTable final_table = reinterpret_cast<FinalTable>(table[index]);
                    if (final_table == nullptr) {
                        final_table = static_cast<FinalTable>(malloc(sizeof(PTE) * pte_size_));
                        table[index] = reinterpret_cast<VAddr>(final_table);
                    }
                    auto pte_index = BitRange<AddrType>(all_page_bits, 0, pte_bits_ - 1);
                    final_table[pte_index] = pte;
                    if (tlb_) {
                        tlb_->CachePage(vaddr, pte);
                    }
                    break;
                }
            }
        }

        void UnMapPage(AddrType vaddr) {
            assert(vaddr % (1 << page_bits_) == 0);
            AddrType all_page_bits = BitRange<AddrType>(vaddr, page_bits_,
                                                        sizeof(AddrType) - addr_width_ - 1);
            AddrType index;
            Table table = reinterpret_cast<Table>(pages_.data());
            for (int i = 0; i < level_; ++i) {
                index = BitRange<AddrType>(all_page_bits, pte_bits_ * (level_ - i - 1),
                                           pte_bits_ * (level_ - i) - 1);
                if (i < level_ - 2) {
                    auto tmp = reinterpret_cast<Table>(table[index]);
                    if (tmp == nullptr) {
                        break;
                    }
                    table = tmp;
                } else {
                    //Final
                    FinalTable final_table = reinterpret_cast<FinalTable>(table[index]);
                    if (final_table == nullptr) {
                        break;
                    }
                    auto pte_index = BitRange<AddrType>(all_page_bits, 0, pte_bits_ - 1);
                    final_table[pte_index] = {};
                    if (tlb_) {
                        tlb_->ClearPageCache(vaddr);
                    }
                    break;
                }
            }
        }

        PTE GetPage(AddrType vaddr) {
            assert(vaddr % (1 << page_bits_) == 0);
            if (tlb_) {
                PTE pte = tlb_->GetPage(vaddr);
                if (pte != PTE{}) {
                    return pte;
                }
            }
            AddrType all_page_bits = BitRange<AddrType>(vaddr, page_bits_,
                                                        sizeof(AddrType) - addr_width_ - 1);
            AddrType index;
            Table table = reinterpret_cast<Table>(pages_.data());
            for (int i = 0; i < level_; ++i) {
                index = BitRange<AddrType>(all_page_bits, pte_bits_ * (level_ - i - 1),
                                           pte_bits_ * (level_ - i) - 1);
                if (i < level_ - 2) {
                    auto tmp = reinterpret_cast<Table>(table[index]);
                    if (tmp == nullptr) {
                        return {};
                    }
                    table = tmp;
                } else {
                    //Final
                    FinalTable final_table = reinterpret_cast<FinalTable>(table[index]);
                    if (final_table == nullptr) {
                        return {};
                    }
                    auto pte_index = BitRange<AddrType>(all_page_bits, 0, pte_bits_ - 1);
                    return final_table[pte_index];
                }
            }
            return {};
        }

        void ReadMemory(const AddrType src_addr, void *dest_buffer, const std::size_t size) {
            AddrType remaining_size = size;
            AddrType page_index = src_addr >> page_bits_;
            AddrType page_offset = src_addr & page_mask_;

            while (remaining_size > 0) {
                const std::size_t copy_amount =
                        std::min(static_cast<std::size_t>(page_size_) - page_offset, remaining_size);
                const AddrType current_vaddr = (page_index << page_bits_) + page_offset;

                auto pte = GetPage(page_index >> page_bits_);

                if (PageReadable(pte)) {
                    std::memset(dest_buffer, 0, copy_amount);
                    InvalidRead(current_vaddr, copy_amount);
                } else {
                    VAddr src_ptr = GetPageStart(pte) + page_offset;
                    std::memcpy(dest_buffer, reinterpret_cast<const void *>(src_ptr),
                                copy_amount);
                    HostReadCallback(src_ptr, copy_amount);
                }

                page_index++;
                page_offset = 0;
                dest_buffer = static_cast<u8*>(dest_buffer) + copy_amount;
                remaining_size -= copy_amount;
            }
        }

        void WriteMemory(const AddrType dest_addr, const void* src_buffer, const std::size_t size) {
            AddrType remaining_size = size;
            AddrType page_index = dest_addr >> page_bits_;
            AddrType page_offset = dest_addr & page_mask_;

            while (remaining_size > 0) {
                const std::size_t copy_amount =
                        std::min(static_cast<std::size_t>(page_size_) - page_offset, remaining_size);
                const AddrType current_vaddr = (page_index << page_bits_) + page_offset;

                auto pte = GetPage(current_vaddr);

                if (!PageWritable(pte)) {
                    InvalidWrite(current_vaddr, copy_amount);
                } else {
                    VAddr dest_ptr = GetPageStart(pte) + page_offset;
                    std::memcpy(reinterpret_cast<void *>(dest_ptr), src_buffer, copy_amount);
                    HostWriteCallback(dest_ptr, copy_amount);
                }

                page_index++;
                page_offset = 0;
                src_buffer = static_cast<const u8*>(src_buffer) + copy_amount;
                remaining_size -= copy_amount;
            }
        }

        template <typename T>
        T Read(const AddrType vaddr) {
            T t;
            ReadMemory(vaddr, &t, sizeof(T));
            return t;
        }

        template <typename T>
        void Write(const AddrType vaddr, const T data) {
            WriteMemory(vaddr, &data, sizeof(T));
        }

        VAddr TopPageTable() {
            return reinterpret_cast<VAddr>(pages_.data());
        }

        SharedPtr<TLB<AddrType, PTE>> Tbl() {
            return tlb_;
        }

        virtual VAddr GetPageStart(PTE &pte) {
            abort();
        };

        virtual bool PageReadable(PTE &pte) {
            return true;
        }

        virtual bool PageWritable(PTE &pte) {
            return true;
        }

        virtual void HostReadCallback(VAddr host_addr, std::size_t size) {};
        virtual void HostWriteCallback(VAddr host_addr, std::size_t size) {};

        virtual void InvalidRead(AddrType vaddr, std::size_t size) {};
        virtual void InvalidWrite(AddrType vaddr, std::size_t size) {};

    protected:
        const u8 addr_width_;
        const u8 page_bits_;
        const AddrType page_size_;
        const AddrType page_mask_;
        u8 pte_bits_;
        std::size_t pte_size_;
        u8 level_;
        std::vector<Table *> pages_;
        SharedPtr<TLB<AddrType, PTE>> tlb_;
    };


}
