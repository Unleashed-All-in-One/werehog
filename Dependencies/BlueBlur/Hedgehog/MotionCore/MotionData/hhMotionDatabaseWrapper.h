#pragma once

#include <Hedgehog/Base/Type/hhSharedString.h>

namespace Hedgehog::Database
{
    class CDatabase;
}

namespace Hedgehog::Motion
{
    class CLightMotionData;
    class CCameraMotionData;
    class CMaterialMotionData;
    class CMorpherMotionData;
    class CUVMotionData;
    class CMotionDatabaseWrapper;

    static inline BB_FUNCTION_PTR(void, __thiscall, fpCDatabaseGetLightMotionData, 0x7599C0,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CLightMotionData>& spLightMotionData, const Hedgehog::Base::CSharedString& name, uint32_t unknown);

    static inline BB_FUNCTION_PTR(void, __thiscall, fpCDatabaseGetCameraMotionData, 0x759960,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CCameraMotionData>& out_spCameraMotionData, const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown);

    static inline BB_FUNCTION_PTR(void, __thiscall, fpCDatabaseGetMaterialMotionData, 0x759720,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CMaterialMotionData>& out_spCameraMotionData, const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown);

    static inline BB_FUNCTION_PTR(void, __thiscall, fpCDatabaseGetMorpherMotionData, 0x759780,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CMorpherMotionData>& out_spCameraMotionData, const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown);

    static inline BB_FUNCTION_PTR(void, __thiscall, fpCDatabaseGetUVMotionData, 0x7597E0,
        CMotionDatabaseWrapper* This, boost::shared_ptr<CUVMotionData>& out_spCameraMotionData, const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown);


    class CMotionDatabaseWrapper
    {
    public:
        Hedgehog::Database::CDatabase* m_pDatabase;

        CMotionDatabaseWrapper(Hedgehog::Database::CDatabase* pDatabase) : m_pDatabase(pDatabase)
        {
        }

        boost::shared_ptr<CLightMotionData> GetLightMotionData(const Base::CSharedString& name, uint32_t unknown = 0)
        {
            boost::shared_ptr<CLightMotionData> spLightMotionData;
            fpCDatabaseGetLightMotionData(this, spLightMotionData, name, unknown);
            return spLightMotionData;
        }
        boost::shared_ptr<CCameraMotionData> GetCameraMotionData(const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown = 0)
        {
            boost::shared_ptr<CCameraMotionData> spCameraMotionData;
            fpCDatabaseGetCameraMotionData(this, spCameraMotionData, in_rName, in_Unknown);
            return spCameraMotionData;
        }
        boost::shared_ptr<CMaterialMotionData> GetMaterialMotionData(const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown = 0)
        {
            boost::shared_ptr<CMaterialMotionData> spMaterialMotionData;
            fpCDatabaseGetMaterialMotionData(this, spMaterialMotionData, in_rName, in_Unknown);
            return spMaterialMotionData;
        }
        boost::shared_ptr<CMorpherMotionData> GetMorpherMotionData(const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown = 0)
        {
            boost::shared_ptr<CMorpherMotionData> spMorpherMotionData;
            fpCDatabaseGetMorpherMotionData(this, spMorpherMotionData, in_rName, in_Unknown);
            return spMorpherMotionData;
        }
        boost::shared_ptr<CUVMotionData> GetUVMotionData(const Hedgehog::Base::CSharedString& in_rName, size_t in_Unknown = 0)
        {
            boost::shared_ptr<CUVMotionData> spUVMotionData;
            fpCDatabaseGetUVMotionData(this, spUVMotionData, in_rName, in_Unknown);
            return spUVMotionData;
        }
    };

    BB_ASSERT_OFFSETOF(CMotionDatabaseWrapper, m_pDatabase, 0x0);
    BB_ASSERT_SIZEOF(CMotionDatabaseWrapper, 0x4);
}