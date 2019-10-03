#pragma once
#include <index_iterator.h>

template<typename OutputType>
struct TypeTraits {};

template<typename T1, typename T2, typename T3>
struct TypeTraits<std::tuple<T1, T2, T3>> {

    using HashType = T1;
    using DocIdType = T2;
    using PositionType = T3;

    enum Indexes : size_t { Hash=0, DocId, Position};
};

template<typename NodeType>
class DiskCachedData {
public:
    using Traits = TypeTraits<NodeType>;
    using IteratorType = IndexIterator<NodeType, sizeof(NodeType)>;

    DiskCachedData(const char* filename) : partitionFile(filename, boost::interprocess::read_write),
                                           mappedPartitionFile(partitionFile, boost::interprocess::read_write) { }

    DiskCachedData() = delete;
    DiskCachedData(const DiskCachedData&) = delete;
    DiskCachedData(DiskCachedData&&) = default;

    DiskCachedData& operator=(const DiskCachedData&) = delete;
    DiskCachedData& operator=(DiskCachedData&&) = default;

    auto begin() const
    {
        return IndexIterator<NodeType, sizeof(NodeType)>(reinterpret_cast<RawMemory>(mappedPartitionFile.get_address()));
    }

    auto end() const
    {
        auto size = mappedPartitionFile.get_size();
        return IndexIterator<NodeType, sizeof(NodeType)>(reinterpret_cast<RawMemory>(mappedPartitionFile.get_address()) + size);

    }
private:
    boost::interprocess::file_mapping partitionFile;
    boost::interprocess::mapped_region mappedPartitionFile;

};
