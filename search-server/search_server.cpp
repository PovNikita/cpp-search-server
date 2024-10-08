#include "search_server.h"
#include <cmath>
using namespace std;

SearchServer::SearchServer() = default;

SearchServer::SearchServer(const string& stop_words_text)
    : SearchServer(
        SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
{
}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
                    const vector<int>& ratings) {
    vector<string> words;
    if(!IsNewIdOk(document_id))
    {
        string error_mes = "Document id isn't correct. ID: "s + to_string(document_id);
        throw invalid_argument(error_mes);
    }
    SplitIntoWordsNoStop(document, words);
    const double inv_word_count = 1.0 / words.size();
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    documents_id_.push_back(document_id);
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(
        raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
        });
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

int SearchServer::GetDocumentId(int index) const{
    return documents_id_.at(index);
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query,
                                int document_id) const {
    const Query query = ParseQuery(raw_query);
    vector<string> matched_words;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return make_tuple(matched_words, documents_.at(document_id).status);
}

[[nodiscard]] bool SearchServer::IsNewIdOk(int document_id) const
{
    if(document_id < 0 || documents_.count(document_id) > 0)
    {
        return false;
    }
    return true;
}

bool SearchServer::IsValidWord(const std::string &word)
{
    return none_of(word.begin(), word.end(), [](char c){
        return c >= '\0' && c < ' ';
    });
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

void SearchServer::SplitIntoWordsNoStop(const string& text, vector<string>& words) const {
    for (const string& word : SplitIntoWords(text)) {
        if(!IsValidWord(word))
        {
            string error_mes = "Word isn't correct. Word: "s;
            error_mes += word;
            throw invalid_argument(error_mes);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
    return rating_sum / static_cast<int>(ratings.size());
}

SearchServer::QueryWord SearchServer::ParseQueryWord(string text) const {
    QueryWord query_word;
    bool is_minus = false;
    // Word shouldn't be empty
    if(IsValidWord(text))
    {
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
            if(text.empty() || text[0] == '-' || !IsValidWord(text))
            {
                string error_mes = "Minus word isn't correct. Word: "s;
                error_mes += ('-' + text);
                throw invalid_argument(error_mes);
            }
            else {
                return {text, is_minus, IsStopWord(text)};
            }
        }
        else
        {
            return {text, is_minus, IsStopWord(text)};
        }
    }
    else{
        string error_mes = "Word isn't correct. Word: "s;
        error_mes += text;
        throw invalid_argument(error_mes);
    }
    return query_word;
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query;
    for (const string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
    }
    return query;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
