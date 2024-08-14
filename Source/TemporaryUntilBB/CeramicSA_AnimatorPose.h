#pragma once

#include <Hedgehog/Base/Type/hhSharedString.h>
#include <Hedgehog/Database/System/hhDatabaseData.h>
#include <Hedgehog/MirageCore/Misc/hhPose.h>

#include "CeramicSA_MiragePose.h"
#include "CeramicSA_hk2010.h"

namespace Ceramic::Animation
{
	class CAnimationPose;
}

namespace Hedgehog::Database
{
    class CDatabase;
}
typedef void (*FPtrUpdateProcedural)(Ceramic::Animation::CAnimationPose*, void*);
class ProceduralData
{
public:
    Sonic::CGameObject* m_pObject = nullptr;
    FPtrUpdateProcedural UpdateProcedural = nullptr;

    // Methods
    static ProceduralData* Get(Ceramic::Animation::CAnimationPose* pose);
    void SetUpdateFunction(void* functionPointer);
};
struct __declspec(align(4)) CAnimationPose_Alternate
{
    struct Map
    {
        // Original properties.
        INSERT_PADDING(0x10) {};

        // Custom extensions.
        ProceduralData procData;
    };

    INSERT_PADDING(0x11C);
    Map* m_pMap;
};

#define _BB_VTABLE_FUNCTION_PTR_NOARG(virtual, returnType, type, function, location, override) \
    virtual returnType function() override \
    { \
        BB_FUNCTION_PTR(returnType, __thiscall, fp##function, location, type This); \
        return fp##function(this); \
    }

#define BB_OVERRIDE_FUNCTION_PTR_CONST_NOARG(returnType, baseType, function, location, ...) \
    _BB_VTABLE_FUNCTION_PTR_NOARG(, returnType, const baseType*, function, location, const override)
#define BB_OVERRIDE_FUNCTION_PTR_CONST(returnType, baseType, function, location, ...) \
    BB_VTABLE_FUNCTION_PTR(, returnType, const baseType*, function, location, const override, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
namespace Ceramic::Animation
{
    class CAnimationCache;
    class CAnimationControlSingle;
    class CAnimationSkeleton;
    class CAnimationPose;
    class CIkRaycastInterface;
    struct SMotionInfo;

    // TODO: MOVE THESE TO THEIR OWN LOCATIONS.
    class hkQsTransform
    {
    public:
        Hedgehog::Math::CVector     m_Position{};
        Hedgehog::Math::CQuaternion m_Rotation{};
        Hedgehog::Math::CVector     m_Scale{};
    };

    class hkaSkeleton
    {
        const char* m_name;

        /// Parent relationship
        int16_t* m_parentIndices;

        /// Number of parent indices, has to be identical to m_numBones
        int32_t m_numParentIndices;

        /// Bones for this skeleton
        Havok2010::hkaBone** m_bones;

        /// Number of bones in the skeleton
        int32_t m_numBones;

        /// The reference pose for this skeleton. This pose is stored in local space.
        hkQsTransform* m_referencePose;

        /// Number of elements in the reference pose array
        int32_t m_numReferencePose;

        /// Floating point track slots. Often used for auxiliary float data or morph target parameters etc.
        /// This defines the target when binding animations to a particular rig.
        char** m_floatSlots;

        /// Number of floating point slots in the skeleton
        int32_t m_numFloatSlots;
    };
    struct SAnimData
    {
        void* m_pSkeleton{}; // type hkaSkeleton
        Havok2010::hkArray<hkQsTransform> m_TransformArray{};
        Havok2010::hkArray<hkQsTransform> m_TransformArray2{};
        BB_INSERT_PADDING(0x04) {};
    };

    class alignas(0x10) CAnimationPose : public Mirage::CPose
    {
    public:
        BB_INSERT_PADDING(0x28) {};
        bool m_Field2C;
        boost::shared_ptr<CAnimationSkeleton> m_spAnimationSkeleton; // 0x30
        Havok2010::hkaAnimatedSkeleton* m_pHavokSkeleton{}; // 0x38
        Mirage::CInstanceInfo* m_pInstanceInfo{}; // 0x3C
        boost::shared_ptr<Mirage::CModelData> m_spModelData; // 0x40
        BB_INSERT_PADDING(0x04) {};

        boost::shared_ptr<CAnimationControlSingle> m_spAnimationControlSingle; // 0x4C
        SAnimData* m_pAnimData{};
        int m_numBones = 0;
        Hedgehog::Math::CMatrix* m_pMatrixListA{};
        Hedgehog::Math::CMatrix* m_pMatrixListB{};
        int m_UnknownMapSize = 0;

        BB_INSERT_PADDING(0x14) {};

        float m_Scale = 0;
        BB_INSERT_PADDING(0x10) {};
        boost::shared_ptr<CIkRaycastInterface> m_spFootIK; // 0x90

        BB_INSERT_PADDING(0x78) {};

        boost::shared_ptr<Hedgehog::Database::CDatabase> m_spDatabase; // 0x110
        BB_INSERT_PADDING(0x18) {};
        boost::shared_ptr<CAnimationCache> m_spAnimationCache; // 0x130
        BB_INSERT_PADDING(0x18) {};

        BB_OVERRIDE_FUNCTION_PTR_CONST_NOARG(size_t, Mirage::CPose, GetMatrixNum, 0x6CCB60);
        BB_OVERRIDE_FUNCTION_PTR_CONST_NOARG(const Hedgehog::Math::CMatrix*, Mirage::CPose, GetMatrixList, 0x6CCBB0);
        BB_OVERRIDE_FUNCTION_PTR_CONST_NOARG(const Hedgehog::Math::CMatrix*, Mirage::CPose, GetPrevMatrixList, 0x6C7A60);
        BB_OVERRIDE_FUNCTION_PTR_CONST(void, Mirage::CPose, GetMatrix, 0x6C7A70, (size_t, in_Index), (Hedgehog::Math::CMatrix&, out_rMatrix))
            BB_OVERRIDE_FUNCTION_PTR(void, Mirage::CPose, SetModel, 0x6C8E90, (const boost::shared_ptr<Mirage::CModelData>&, in_spModelData))
            BB_OVERRIDE_FUNCTION_PTR(void, Mirage::CPose, SetInstanceInfo, 0x6C7950, (Mirage::CInstanceInfo*, in_pInstanceInfo))

            static boost::shared_ptr<CAnimationCache> CreateAnimationCache();

        CAnimationPose(const bb_null_ctor&) {}
        CAnimationPose(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, const Hedgehog::Base::CSharedString& in_rName);

        void AddMotionInfo(SMotionInfo* in_pMotionInfo, size_t in_NumMotionInfo);
        void Update(float in_DeltaTime);
    };

    BB_ASSERT_OFFSETOF(CAnimationPose, m_spAnimationSkeleton, 0x30);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_pHavokSkeleton, 0x38);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_pInstanceInfo, 0x3C);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_spModelData, 0x40);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_Scale, 0x7C);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_spFootIK, 0x90);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_spDatabase, 0x110);
    BB_ASSERT_OFFSETOF(CAnimationPose, m_spAnimationCache, 0x130);
    BB_ASSERT_SIZEOF(CAnimationPose, 0x150);
}

#include "CeramicSA_AnimatorPose.inl"