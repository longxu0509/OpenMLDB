/*
 * Copyright 2021 4Paradigm
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SRC_STORAGE_WINDOW_ITERATOR_H_
#define SRC_STORAGE_WINDOW_ITERATOR_H_

#include <memory>
#include <string>
#include "storage/segment.h"
#include "base/concurrentlist.h"
#include "vm/catalog.h"

namespace openmldb {
namespace storage {

class MemTableWindowIterator : public ::hybridse::vm::RowIterator {
 public:
    MemTableWindowIterator(BaseTimeEntriesIterator* it, ::openmldb::storage::TTLType ttl_type, uint64_t expire_time,
                           uint64_t expire_cnt)
        : it_(it), record_idx_(1), expire_value_(expire_time, expire_cnt, ttl_type), row_() {}

    ~MemTableWindowIterator();

    bool Valid() const override;

    void Next() override;

    const uint64_t& GetKey() const override;

    const ::hybridse::codec::Row& GetValue() override;

    void Seek(const uint64_t& key) override;

    void SeekToFirst() override;

    bool IsSeekable() const override { return true; }

 private:
    BaseTimeEntriesIterator* it_;
    uint32_t record_idx_;
    TTLSt expire_value_;
    ::hybridse::codec::Row row_;
};

class MemTableKeyIterator : public ::hybridse::vm::WindowIterator {
 public:
    MemTableKeyIterator(Segment** segments, uint32_t seg_cnt, ::openmldb::storage::TTLType ttl_type,
                        uint64_t expire_time, uint64_t expire_cnt, uint32_t ts_index);

    ~MemTableKeyIterator() override;

    void Seek(const std::string& key) override;

    void SeekToFirst() override;

    void Next() override;

    bool Valid() override;

    std::unique_ptr<::hybridse::vm::RowIterator> GetValue() override;
    ::hybridse::vm::RowIterator* GetRawValue() override;

    const hybridse::codec::Row GetKey() override;

 private:
    void NextPK();

 private:
    Segment** segments_;
    uint32_t const seg_cnt_;
    uint32_t seg_idx_;
    KeyEntries::Iterator* pk_it_;
    BaseTimeEntriesIterator* it_;  //  TimeEntries::Iterator*
    ::openmldb::storage::TTLType ttl_type_;
    uint64_t expire_time_;
    uint64_t expire_cnt_;
    Ticket ticket_;
    uint32_t ts_idx_;
};

}  // namespace storage
}  // namespace openmldb

#endif  // SRC_STORAGE_WINDOW_ITERATOR_H_
