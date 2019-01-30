#include <wdb/wdb_code_gen.h>
#include <wabt/src/binary-reader-ir.h>
#include <wabt/src/ir.h>
#include <wabt/src/validator.h>
#include <wabt/src/generate-names.h>
#include <wabt/src/apply-names.h>
#include <wabt/src/stream.h>
#include <fstream>

namespace wdb {
    wabt::Result WdbCodeGen::SetupCode(std::vector<uint8_t> *fileData) {
        // Prepare result value
        wabt::Result result;
        // Read binary and populate main module
        wabt::ReadBinaryOptions binaryOptions;
        wabt::Errors errors;
        result = wabt::ReadBinaryIr("", fileData->data(), fileData->size(),
                binaryOptions, &errors, &m_mainModule);
        if(result == wabt::Result::Ok) {
            // Validate main module
            wabt::Features features;
            wabt::ValidateOptions validateOptions(features);
            result = ValidateModule(&m_mainModule, &errors, validateOptions);
            if(result != wabt::Result::Ok) {
                // Generate names
                result = wabt::GenerateNames(&m_mainModule);
                ApplyNames(&m_mainModule);
            }
        }
        return result;
    }

    std::string WdbCodeGen::GetWat(wabt::WriteWatOptions watOptions) {
        std::string watCode;
        wabt::MemoryStream stream;
        wabt::Result result = WriteWat(&stream, &m_mainModule, watOptions);
        if (result == wabt::Result::Ok) {
            auto data = stream.output_buffer().data;
            watCode = std::string(data.begin(), data.end());
        }
        return watCode;
    }
}