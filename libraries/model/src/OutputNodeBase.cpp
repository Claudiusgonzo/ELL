////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNodeBase.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNodeBase.h"
#include "IRMapCompiler.h"
#include "OutputNodeBase.h"

namespace ell
{
namespace model
{
    OutputNodeBase::OutputNodeBase(InputPortBase& input, OutputPortBase& output, const MemoryShape& shape)
        : CompilableNode({ &input }, { &output }), _inputBase(input), _outputBase(output)
    {
    }

    OutputNodeBase::OutputNodeBase(const std::vector<InputPortBase*>& inputs, OutputPortBase& output, const MemoryShape& shape)
        : CompilableNode(inputs, { &output }), _inputBase(*inputs.at(0)), _outputBase(output)
    {
    }

    MemoryShape OutputNodeBase::GetShape() const 
    { 
        return _outputBase.GetMemoryLayout().GetActiveSize();
    }

    void OutputNodeBase::SetShape(const MemoryShape& shape) 
    { 
        _outputBase.SetMemoryLayout({shape});
    } 

    ell::utilities::ArchiveVersion OutputNodeBase::GetArchiveVersion() const
    {
        return {ell::utilities::ArchiveVersionNumbers::v2};
    }

    void OutputNodeBase::Compile(IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        assert(GetPortVariableType(_inputBase) == GetPortVariableType(_outputBase));

        auto output = function.LocalArray(compiler.EnsurePortEmitted(_outputBase));
        // check if the output variable is null.
        function.If(ell::emitters::TypedComparison::notEquals, output, function.NullPointer(output.value->getType()->getPointerElementType()->getPointerTo()), [output, &compiler, this](emitters::IRFunctionEmitter& function) {
            auto inputElements = _inputBase.GetInputElements();
            int rangeStart = 0;
            for (auto range : inputElements.GetRanges())
            {
                auto input = function.LocalArray(compiler.EnsurePortEmitted(*range.ReferencedPort()));
                auto rangeSize = range.Size();
                function.For(rangeSize, [range, rangeStart, input, output](emitters::IRFunctionEmitter& function, auto i) {
                    auto inputIndex = i + static_cast<int>(range.GetStartIndex());
                    auto outputIndex = i + rangeStart;
                    output[outputIndex] = input[inputIndex];
                });
                rangeStart += rangeSize;
            }
        });
    }
}
}
