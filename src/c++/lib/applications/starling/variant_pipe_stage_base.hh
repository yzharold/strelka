// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2016 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

/// \author John Duddy

#pragma once
#include "gvcf_locus_info.hh"


/// base class used for snv/indel processing pipeline from "raw" calls to gVCF
/// output.
///
/// Design is based on passign site/indel_info ownership down the pipe via unique_ptr/move
///
class variant_pipe_stage_base
{
public:
    virtual void process(std::unique_ptr<GermlineSiteLocusInfo> si)
    {
        if (_sink) _sink->process(std::move(si));
    }
    virtual void process(std::unique_ptr<GermlineIndelLocusInfo> ii)
    {
        if (_sink) _sink->process(std::move(ii));
    }

    void flush()
    {
        flush_impl();
        if (_sink)
            _sink->flush();
    }

    explicit variant_pipe_stage_base(std::shared_ptr<variant_pipe_stage_base> sink) : _sink(sink) {}

    virtual ~variant_pipe_stage_base() {}

protected:
    variant_pipe_stage_base() : _sink(nullptr) {}

    virtual void flush_impl() {}

    template <class TDerived, class TBase>
    static std::unique_ptr<TDerived> downcast(std::unique_ptr<TBase> basePtr)
    {
        TDerived* ptr(dynamic_cast<TDerived*>(basePtr.release()));
        if (ptr != nullptr)
        {
            return std::unique_ptr<TDerived>(ptr);
        }
        throw std::bad_cast();
    }

    std::shared_ptr<variant_pipe_stage_base> _sink;
};

