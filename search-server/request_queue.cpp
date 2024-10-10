#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) 
: search_server_ (search_server)
{
        
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, 
                    [status](int document_id, DocumentStatus document_status, int rating) {
                    return document_status == status;});
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    return AddFindRequest(raw_query, 
                    [](int document_id, DocumentStatus document_status, int rating) {
                    return document_status == DocumentStatus::ACTUAL;});
}

int RequestQueue::GetNoResultRequests() const {
    return no_results_requests_;
}

void RequestQueue::AddRequestQuery(bool is_empty_request)
{
    QueryResult request;
    request.time_ = current_time_in_minutes_;
    request.is_empty_request_ = is_empty_request;
    requests_.push_back(request);
}