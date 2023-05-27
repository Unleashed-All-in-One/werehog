namespace Sonic
{
    class CRigidBody
    {
    public:
        virtual ~CRigidBody() = default;

        Havok::RigidBody* m_pHkpRigidBody;


        virtual void* GetCollider(void* a1) { return a1; }
        virtual void* fn02(void* a1) { return a1; }
        virtual void ApplyPropertyID(int id, uint64_t* weird) {}
        virtual void fn04(int a, int b) {}
        virtual void fn05(int a, int b) {}
        virtual void GetPropertyValue(uint64_t* weird, int hash) {}
        virtual bool VerifyPropertyID(int hash) { return false; }

        void ApplyPropertyID(int id, int value)
        {
            struct
            {
                int value;
                int buffer;
            } data;
            data.value = value;
            ApplyPropertyID(id, (uint64_t*)&data);
        }

        void AddBoolProperty(int hashedName, int id)
        {
            if (!VerifyPropertyID(hashedName))
                return;

            uint64_t data = 0;
            GetPropertyValue(&data, hashedName);

            data = data != 0;
            ApplyPropertyID(id, &data);
        }

        bool GetBoolProperty(int id)
        {
            if (!VerifyPropertyID(id))
                return false;

            uint64_t data = 0;
            GetPropertyValue(&data, id);

            return data != 0;
        }

        Havok::Shape* GetShape() const
        {
            return m_pHkpRigidBody->m_Collideable.m_Shape;
        }
    };
}

 