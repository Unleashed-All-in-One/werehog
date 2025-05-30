#pragma once

#include <Hedgehog/Base/hhObject.h>

namespace Ceramic::Mirage
{
    class CModelData;
    class CInstanceInfo;

    class CPose : public Hedgehog::Base::CObject
    {
    public:
        virtual ~CPose() = default;

        virtual size_t GetMatrixNum() const = 0;
        virtual const Hedgehog::Math::CMatrix* GetMatrixList() const = 0;
        virtual const Hedgehog::Math::CMatrix* GetPrevMatrixList() const = 0;
        virtual void GetMatrix(size_t in_Index, Hedgehog::Math::CMatrix& out_rMatrix) const;

        virtual void SetModel(const boost::shared_ptr<Mirage::CModelData>& in_spModelData) {}
        virtual void SetInstanceInfo(Mirage::CInstanceInfo* in_pInstanceInfo) {}
    };
}

#include "CeramicSA_MiragePose.inl"