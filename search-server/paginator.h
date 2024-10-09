#pragma once
#include <iostream>
#include <vector>
#include <cmath>

template <typename Iterator>
class PageRange
{
    public:

    PageRange() = default;

    PageRange(Iterator begin, Iterator end)
    :first_doc_(begin), last_doc_(end)
    {

    }

    Iterator begin() const
    {
        return first_doc_;
    }

    Iterator end() const
    {
        return last_doc_;
    }

    size_t size() const
    {
        return distance(first_doc_, last_doc_);
    }

    private:
    Iterator first_doc_;
    Iterator last_doc_;
};

template <typename Iter>
std::ostream& operator <<(std::ostream& output, const PageRange<Iter> page_range)
{
    Iter it = page_range.begin();
    Iter it_end = page_range.end();
    while (it < it_end)
    {
        output << *it;
        advance(it, 1);
    }
    return output;
}

template <typename Iterator>
class Paginator{
    public:
    Paginator() = default;

    Paginator(Iterator begin, Iterator end, size_t size)
    :page_size_(size)
    {
        int number_of_pages = std::ceil(static_cast<float>(distance(begin, end)) / static_cast<float>(size));
        for(int i = 0; i < number_of_pages-1; ++i)
        {   
            Iterator end = next(begin, size);
            page_.push_back(PageRange(begin, end));
            begin = end;
        }
        page_.push_back({begin, end});
        current_page_ = page_.front();
    }

    auto begin() const{
        return page_.begin();
    }

    auto end() const{
        return page_.end();
    }

    private:
    std::vector<PageRange<Iterator>> page_;
    PageRange<Iterator> current_page_;
    size_t page_size_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}