#pragma once

#include <vector>
#include <string>
#include <queue>
#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        u_int16_t time_ = 0;
        bool is_empty_request_ = true;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    u_int16_t current_time_in_minutes_ = 0;
    int no_results_requests_ = 0;

    void AddRequestQuery(bool is_empty_request);
};


template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    using namespace std;
    ++current_time_in_minutes_;
    vector<Document> docs_from_serv_;
    if(current_time_in_minutes_ / static_cast<float>(min_in_day_) > 1.0)
    {
        if(requests_.front().is_empty_request_)
        {
            --no_results_requests_;
        }
        requests_.pop_front();
    }
    docs_from_serv_ = search_server_.FindTopDocuments(raw_query, document_predicate);
    if(docs_from_serv_.size() == 0)
    {
        AddRequestQuery(true);
        ++no_results_requests_;
    }
    else
    {
        AddRequestQuery(false);
    }
    return docs_from_serv_;
}