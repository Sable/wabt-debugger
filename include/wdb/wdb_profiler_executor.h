#ifndef WDB_WDB_PROFILER_EXECUTOR_H
#define WDB_WDB_PROFILER_EXECUTOR_H

#include <wdb/wdb_executor.h>

namespace wdb {
    class WdbProfilerExecutor : public WdbExecutor {
    public:

        // Sorting methods
        enum Sort {
            OPCODE_ASC,
            TOTAL_COUNT_ASC,
            TOTAL_TIME_ASC,
            AVG_TIME_ASC,
            OPCODE_DESC,
            TOTAL_COUNT_DESC,
            TOTAL_TIME_DESC,
            AVG_TIME_DESC
        };

        // Profiling entry struct
        struct ProfilerEntry {
            wabt::Opcode opcode;
            long count = 0;
            long totalTime = 0;
            wabt::Opcode GetOpcode() const { return opcode; }
            long GetCount() const { return count; }
            long GetTotalTime() const { return totalTime; }
            double GetAverageTime() const;
        };

        /**
         * Create a profiler executor
         * @param options
         */
        WdbProfilerExecutor(WdbExecutor::Options options);

        /**
         * Get profiler map
         * @return profiler map
         */
        std::map<wabt::Opcode, ProfilerEntry> GetProfilerMap() const { return m_profilerMap; };

        /**
         * Get vector profiler entry sorted
         * @param listSort
         * @return vector of sorted profiler entries
         */
        std::vector<ProfilerEntry> GetProfilerSorted(Sort sort);

        /**
         * Execute instruction and record profiler information
         * @return result
         */
        wabt::Result Execute();
    private:
        // Profiler map
        std::map<wabt::Opcode, ProfilerEntry> m_profilerMap;
    };
}

#endif