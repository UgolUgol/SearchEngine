#pragma once

#include <unordered_map>
#include <index.h>
#include <numeric>

template<typename TerminType, typename Ranker>
class RankingMaster {
public:

    void addTermin(const TerminType& term)
    {
        rankingMetrics.emplace(term, 0.0);
    }

    template<typename... RankParameters>
    bool updateTerminMetric(const TerminType& term, RankParameters&&... paramters)
    {

        auto terminNode = rankingMetrics.find(term);
        if(terminNode == std::end(rankingMetrics)) {

            return false;

        }

        terminNode->second = Ranker::calculate(std::forward<RankParameters>(paramters)...);
        return true;

    }

    double getMetrics() const
    {

        return std::accumulate(std::cbegin(rankingMetrics), std::cend(rankingMetrics), 0.0,
                [](double sum, const auto& metric) { return sum + metric.second; } );

    }



private:
    std::unordered_map<TerminType, double> rankingMetrics;
};