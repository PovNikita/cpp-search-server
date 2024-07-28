// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь: 271

// Закомитьте изменения и отправьте их в свой репозиторий.


#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Query{
    set<string> query_words;
    set<string> minus_words;
};

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        for(string word : words)
        {
            (docs_[word])[document_id] += (1./words.size());
        }
    }

    vector<Document> FindTopDocuments(const string& raw_query) const{
        const Query query_set = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_set);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    int GetNumberOfDocs() {
    document_count_ = ReadLineWithNumber();
    return document_count_;
}

private:
    map<string, map<int, double>> docs_;
    set<string> stop_words_;

    int document_count_ = 0;
    
    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const {
        Query query_set;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-')
            {
                query_set.minus_words.insert(word.substr(1));
                query_set.query_words.insert(word.substr(1));
            }
            query_set.query_words.insert(word);
        }
        return query_set;
    }

    vector<Document> FindAllDocuments(const Query& query_set) const {
        vector<Document> matched_documents;
        map<int, double> pre_matched_documents;
        for(string word : query_set.query_words)
        {
            if(docs_.count(word))
            {
                //Calculating TF-IDF for every word from query
                for(auto [id, tf_idf] : docs_.at(word))
                {
                    tf_idf *= log(static_cast<double>(document_count_)/static_cast<double>(docs_.at(word).size()));
                    //Summing relevance
                    pre_matched_documents[id] += tf_idf;
                }
            }
        }
        for(string word : query_set.minus_words)
        {
            if(docs_.count(word))
            {  
                for(auto [id, tf] : docs_.at(word))
                {
                    if(pre_matched_documents.count(id))
                        pre_matched_documents.erase(id);
                }
            }
        }
        Document doc;
        for(auto [id, value]: pre_matched_documents)
        {
            doc.id = id;
            doc.relevance = value;
            matched_documents.push_back(doc);
        }
        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = search_server.GetNumberOfDocs();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    setlocale(LC_ALL, "");
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
    return 0;
}