namespace Ceramic::Mirage
{
    inline void Ceramic::Mirage::CPose::GetMatrix(size_t in_Index, Hedgehog::Math::CMatrix& out_rMatrix) const
    {
        out_rMatrix = GetMatrixList()[in_Index];
    }
}