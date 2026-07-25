#include <pybind11/pybind11.h>
#include <tuple>
#include "archive_core.h"

namespace py = pybind11;

PYBIND11_MODULE(archive_py, m)
{
    py::enum_<Result>(m, "Result")
        .value("Ok", Result::Ok)
        .value("CannotOpenInput", Result::CannotOpenInput)
        .value("CannotCreateOutput", Result::CannotCreateOutput)
        .value("EmptyFile", Result::EmptyFile)
        .value("InvalidFormat", Result::InvalidFormat)
        .value("CorruptedArchive", Result::CorruptedArchive)
        .value("SameFile", Result::SameFile);

    py::class_<CompressStats>(m, "CompressStats")
        .def(py::init<>())
        .def_readonly("original_size", &CompressStats::originalSize)
        .def_readonly("compressed_size", &CompressStats::compressedSize)
        .def("ratio", &CompressStats::ratio);

    py::class_<RLE>(m, "RLE")
        .def(py::init<>())
        .def("compress", [](RLE& self, const std::string& input, const std::string& output)
        {
            CompressStats stats;
            Result r = self.compress(input, output, stats);
            return std::make_tuple(r, stats);
        })
        .def("decompress", &RLE::decompress);

    m.def("result_to_string", &resultToString);
    m.def("peek_original_name", &peekOriginalName);
}