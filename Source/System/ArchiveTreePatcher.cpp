
vector<ArchiveDependency> ArchiveTreePatcher::m_archiveDependencies = {};
vector<string> ArchiveTreePatcher::m_languageArchives = {};

HOOK(bool, __stdcall, ArchiveTreePatcher_ParseArchiveTree, 0xD4C8E0, void* A1, char* pData, const size_t size, void* pDatabase)
{
    std::string str;
    {
        std::stringstream stream;

        for (ArchiveDependency const& node : ArchiveTreePatcher::m_archiveDependencies)
        {
            stream << "  <Node>\n";
            stream << "    <Name>" << node.m_archive << "</Name>\n";
            stream << "    <Archive>" << node.m_archive << "</Archive>\n";
            stream << "    <Order>" << 0 << "</Order>\n";
            stream << "    <DefAppend>" << node.m_archive << "</DefAppend>\n";

            for (string const& dependency : node.m_dependencies)
            {
                stream << "    <Node>\n";
                stream << "      <Name>" << dependency << "</Name>\n";
                stream << "      <Archive>" << dependency << "</Archive>\n";
                stream << "      <Order>" << 0 << "</Order>\n";
                stream << "    </Node>\n";
            }

            stream << "  </Node>\n";
        }

        str = stream.str();
    }

    const size_t newSize = size + str.size();
    const std::unique_ptr<char[]> pBuffer = std::make_unique<char[]>(newSize);
    memcpy(pBuffer.get(), pData, size);

    char* pInsertionPos = strstr(pBuffer.get(), "<Include>");

    memmove(pInsertionPos + str.size(), pInsertionPos, size - (size_t)(pInsertionPos - pBuffer.get()));
    memcpy(pInsertionPos, str.c_str(), str.size());

    bool result;
    {
        result = originalArchiveTreePatcher_ParseArchiveTree(A1, pBuffer.get(), newSize, pDatabase);
    }

    return result;
}

boost::shared_ptr<hh::db::CRawData>* __fastcall ArchiveTreePatcher_GetRawDataImpl
(
    hh::db::CDatabase* This,
    void* Edx,
    boost::shared_ptr<hh::db::CRawData>& rawData,
    const hh::base::CSharedString& name,
    const bool flag)
{
    hh::db::fpCDatabaseGetRawData(This, rawData, name, flag);
    if (name != "LanguageTree.xml" || !rawData || !rawData->m_spData)
        return &rawData;

    std::string str;
    {
        std::stringstream stream;
        for (string const& archive : ArchiveTreePatcher::m_languageArchives)
        {
            stream << "<Archive>" << archive << "</Archive>\n";
        }
        str = stream.str();
    }

    const char* const appendData = str.c_str();
    const size_t appendDataSize = strlen(appendData);

    const size_t newSize = rawData->m_DataSize + appendDataSize;
    const boost::shared_ptr<char[]> buffer = boost::make_shared<char[]>(newSize);
    memcpy(buffer.get(), rawData->m_spData.get(), rawData->m_DataSize);

    char* insertionPos = strstr(buffer.get(), "</Language>");

    memmove(insertionPos + appendDataSize, insertionPos, rawData->m_DataSize - (size_t)(insertionPos - buffer.get()));
    memcpy(insertionPos, appendData, appendDataSize);

    rawData = boost::make_shared<hh::db::CRawData>();
    rawData->m_Flags = hh::db::eDatabaseDataFlags_IsMadeAll;
    rawData->m_spData = buffer;
    rawData->m_DataSize = newSize;

    return &rawData;
}

void ArchiveTreePatcher::applyPatches()
{
    m_archiveDependencies.push_back(ArchiveDependency("EvilSonic", { "Sonic" }));
    if (!m_archiveDependencies.empty())
    {
        INSTALL_HOOK(ArchiveTreePatcher_ParseArchiveTree);
    }
}
