#ifndef WDB_WDB_CODE_GEN_H
#define WDB_WDB_CODE_GEN_H

#include <wabt/src/binary-reader.h>
#include <wabt/src/wat-writer.h>
#include <wabt/src/ir.h>

namespace wdb {
    class WdbCodeGen {
    public:
        /**
         * Set up environment
         * @param fileName
         * @param fileData
         * @return result
         */
        wabt::Result SetupCode(std::vector<uint8_t> *fileData);

        /**
         * Get wat code
         * @param watOptions
         * @return wat code
         */
        std::string GetWat(wabt::WriteWatOptions watOptions = wabt::WriteWatOptions());
    private:
        wabt::Module m_mainModule;
    };
}

#endif
