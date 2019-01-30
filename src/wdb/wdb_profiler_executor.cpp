#include <wdb/wdb_profiler_executor.h>
#include <wabt/src/interp/interp-internal.h>
#include <chrono>
#include <utility>

namespace wdb {
    WdbProfilerExecutor::WdbProfilerExecutor(wdb::WdbExecutor::Options options) : WdbExecutor(std::move(options)) {}

    double WdbProfilerExecutor::ProfilerEntry::GetAverageTime() const {
        if(count == 0) {
            return 0;
        }
        return (double) totalTime / count;
    }

    std::vector<WdbProfilerExecutor::ProfilerEntry> WdbProfilerExecutor::GetProfilerSorted(Sort sort) {
        // Define sorting function
        auto sortingFunction = [=](ProfilerEntry const &a, ProfilerEntry const &b) {
            switch (sort) {
                case OPCODE_ASC:
                    return strcmp(a.opcode.GetName(), b.opcode.GetName()) < 0;
                case TOTAL_COUNT_ASC:
                    return a.GetCount() < b.GetCount();
                case TOTAL_TIME_ASC:
                    return a.GetTotalTime() < b.GetTotalTime();
                case AVG_TIME_ASC:
                    return a.GetAverageTime() < b.GetAverageTime();
                case OPCODE_DESC:
                    return strcmp(a.GetOpcode().GetName(), b.GetOpcode().GetName()) > 0;
                case TOTAL_COUNT_DESC:
                    return a.GetCount() > b.GetCount();
                case TOTAL_TIME_DESC:
                    return a.GetTotalTime() > b.GetTotalTime();
                case AVG_TIME_DESC:
                    return a.GetAverageTime() > b.GetAverageTime();
            }
        };
        // Prepare profiler entries vector
        std::vector<ProfilerEntry> profilerEntries;
        for(auto pair : m_profilerMap) {
            profilerEntries.emplace_back(pair.second);
        }
        // Sort entries based on the defined function
        std::sort(profilerEntries.begin(), profilerEntries.end(), sortingFunction);
        return profilerEntries;
    }

    wabt::Result WdbProfilerExecutor::Execute() {
        if (CanRun()) {
            wabt::interp::Result result = wabt::interp::Result::Ok;
            // Keep executing instructions
            while (result == wabt::interp::Result::Ok) {
                // Create a temp pc
                const uint8_t *istream = m_env->istream().data.data();
                const uint8_t *tmpPc = &istream[m_thread->pc()];
                // Fetch opcode at this pc
                wabt::Opcode opcode = wabt::interp::ReadOpcode(&tmpPc);
                // Start measuring the execution time
                std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
                // Run next instruction
                result = m_thread->Run(1);
                // Record the execution time
                std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
                // Create entry if not already created
                if (m_profilerMap.find(opcode) == m_profilerMap.end()) {
                    m_profilerMap[opcode] = ProfilerEntry();
                    m_profilerMap[opcode].opcode = opcode;
                }
                m_profilerMap[opcode].count++;
                m_profilerMap[opcode].totalTime +=
                        std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
            }
            // Main function has returned
            if (result == wabt::interp::Result::Returned) {
                SetMainFunctionReturned();
                return wabt::Result::Ok;
            }
        }
        return wabt::Result::Error;
    }
}