void trim_cruft(std::string& buffer)
{
	static const char cruft[] = "\n\r";

	buffer.erase(buffer.find_last_not_of(cruft) + 1);
}
bool isPartOf(const char* w1, const char* w2)
{
	int i = 0;
	int j = 0;


	while (w1[i] != '\0') {
		if (w1[i] == w2[j])
		{
			int init = i;
			while (w1[i] == w2[j] && w2[j] != '\0')
			{
				j++;
				i++;
			}
			if (w2[j] == '\0') {
				return true;
			}
			j = 0;
		}
		i++;
	}
	return false;
}
std::string XMLParser::CLAWPARTICLE = "slash";
std::string toCheck;
std::string alreadyChecked;
std::vector<Motion> XMLParser::animationTable;
std::vector<std::string> alreadyRegistered;
std::vector<Motion> localMotionFileList;
std::vector<WerehogAttackNew> XMLParser::attacks;
std::vector<WerehogAttackNew> XMLParser::starterAttacks;
////RHand,
//LHand,
//RLeg,
//LLeg,
//MiddleHand,
//Head,
//Hips,
//MiddleLeg
std::vector<std::string> collisionBoneNames = { "RHand", "LHand", "RLeg", "LLeg", "MiddleHand", "Head", "Hips", "MiddleLeg"};
WerehogAttackNew ParseActionNode(rapidxml::xml_node<>* node, rapidxml::xml_node<>* parent)
{
	WerehogAttackNew attack = WerehogAttackNew();
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		const char* name = child->name();
		std::stringstream ss(child->value());
		if (isPartOf(name, "ActionName"))
			attack.ActionName = std::string(child->value());
		if (isPartOf(name, "MotionName"))
			attack.MotionName = std::string(child->value());
		if (isPartOf(name, "ValidLevel_Min"))
			attack.ValidLevel_Min = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ValidLevel_Max"))
			attack.ValidLevel_Max = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ValidCommon"))
			attack.ValidCommon = child->value() == "true" ? true : false;
		if (isPartOf(name, "ValidBerserker"))
			attack.ValidBerserker = child->value() == "true" ? true : false;
		if (isPartOf(name, "KEY__YDown"))
			attack.KEY__YDown = std::string(child->value());
		if (isPartOf(name, "KEY__XDown"))
			attack.KEY__XDown = std::string(child->value());
		if (isPartOf(name, "KEY__ADown"))
			attack.KEY__ADown = std::string(child->value());
		if (isPartOf(name, "KEY__Land"))
			attack.KEY__Land = std::string(child->value());
		if (isPartOf(name, "KEY__AirCombo"))
			attack.KEY__AirCombo = std::string(child->value());
		if (isPartOf(name, "KEY__End"))
			attack.KEY__End = std::string(child->value());
		if (isPartOf(name, "Guard"))
			attack.Guard = child->value() == "true" ? true : false;
		if (isPartOf(name, "Avoid"))
			attack.Avoid = child->value() == "true" ? true : false;
		if (isPartOf(name, "KEY__StartFrame"))
			attack.KEY__StartFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "KEY__EndFrame"))
			attack.KEY__EndFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "WaitEndMotionEndFrame"))
			attack.WaitEndMotionEndFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "WaitEndMotionSpeed"))
			attack.WaitEndMotionSpeed = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "EndMotionSpeed"))
			attack.EndMotionSpeed = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "LandStartFrame"))
			attack.LandStartFrame = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "ActionValidHeightMin"))
			attack.ActionValidHeightMin = reinterpret_cast<int>(child->value());
		if (isPartOf(name, "IsGravity"))
			attack.IsGravity = child->value() == "true" ? true : false;
		
	}
	return attack;
}

Motion ParseMotionNode(rapidxml::xml_node<>* node)
{
	Motion returned;
	for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
	{
		const char* name = child->name();
		std::string nameS = std::string(name);
		if (nameS == "MotionName")
			returned.MotionName = child->value();
		if (nameS == "FileName")
			returned.FileName = child->value();
		if (nameS == "MotionBlendTimeS")
			returned.MotionBlendTimeS = std::stof(child->value());
		if (nameS == "MotionBlendTimeE")
			returned.MotionBlendTimeE = std::stof(child->value());
		if (nameS == "MotionBlendTimeEIdle")
			returned.MotionBlendTimeEIdle = std::stof(child->value());
		if (nameS == "MotionMoveSpeedRatio")
			returned.MotionMoveSpeedRatio = std::stof(child->value());
		if (nameS == "MotionMoveSpeedRatio_Y")
			returned.MotionMoveSpeedRatio_Y = std::stof(child->value());
		//21 because Unleashed caps it at 21
		for (size_t i = 1; i < 21; i++)
		{
			//This is a custom struct, want to do this to make it easier to process it
			if (returned.MotionMoveSpeedRatio_H.size() <= i)
				returned.MotionMoveSpeedRatio_H.push_back({});
			//Inserting at i-1 since it starts at 1
			if (name == std::format("MotionMoveSpeedRatioFrameStart_{0}", i))
			{
				returned.MotionMoveSpeedRatio_H.at(i).FrameStart = std::stof(child->value());
				returned.MotionMoveSpeedRatioFrameStart.insert(returned.MotionMoveSpeedRatioFrameStart.begin() + i - 1, std::stof(child->value()));
			}
			if (name == std::format("MotionMoveSpeedRatioFrame_{0}", i))
			{
				returned.MotionMoveSpeedRatio_H.at(i).FrameValue = std::stof(child->value());
				returned.MotionMoveSpeedRatioFrame.insert(returned.MotionMoveSpeedRatioFrame.begin() + i - 1, std::stof(child->value()));
			}
			if (name == std::format("MotionMoveSpeedRatioFrameY_{0}", i))
			{
				returned.MotionMoveSpeedRatio_H.at(i).FrameValue_Y = std::stof(child->value());
				returned.MotionMoveSpeedRatioFrameY.insert(returned.MotionMoveSpeedRatioFrameY.begin() + i - 1, std::stof(child->value()));
			}
		}
		
		if (nameS == "MotionFirstSpeed") //dont think about it too much
			returned.MotionFirstSpeed = std::stof(std::string(child->value()));
		if (nameS == "MotionSpeed_FirstFrame") //dont think about it too much
			returned.MotionSpeed_FirstFrame = std::stof(std::string(child->value()));
		if (nameS == "MotionSpeed_MiddleFrame") //dont think about it too much
			returned.MotionSpeed_MiddleFrame = std::stof(std::string(child->value()));
		if (nameS == "Effect")
		{
			for (rapidxml::xml_node<>* child2 = child->first_node(); child2; child2 = child2->next_sibling())
			{
				const char* name2 = child2->name();
				if (isPartOf(name2, "RFEffect_Name1") || isPartOf(name2, "REffect_Name1"))
				{
					returned.Effect.REffect_Name1 = child2->value();
				}
				if (isPartOf(name2, "LFEffect_Name1") || isPartOf(name2, "LEffect_Name1"))
				{
					returned.Effect.LEffect_Name1 = child2->value();
				}
				if (isPartOf(name2, "RClaw_Start1") && child2->value() != nullptr)
				{
					returned.Effect.REffect_Name1 = XMLParser::CLAWPARTICLE;
				}
				if (isPartOf(name2, "LClaw_Start1") && child2->value() != nullptr)
				{
					returned.Effect.LEffect_Name1 = XMLParser::CLAWPARTICLE;
				}
			}
		}
		if (nameS == "Collision")
		{
			
			for (rapidxml::xml_node<>* child2 = child->first_node(); child2; child2 = child2->next_sibling())
			{
				//example
					
				//<DebugDraw>true</DebugDraw>
				//<Size1>1.f</Size1>
				//<SizeDelta1>0.25f</SizeDelta1>
				//<RHandStart1>1</RHandStart1>
				//<RHandEnd1>4</RHandEnd1>
				//<Size2>1.f</Size2>
				//<SizeDelta2>0.25f</SizeDelta2>
				//<HandStart2>5</LHandStart2>
				//<LHandEnd2>8</LHandEnd2>

				//list of possible bone names:
				//RHandStart
				//RHandEnd
				//LHandStart
				//LHandEnd
				//RLegStart
				//RLegEnd
				//LLegStart
				//LLegEnd
				//MiddleHandStart
				//MiddleHandEnd
				//HeadStart
				//HeadEnd
				//HipsStart
				//HipsEnd
				//MiddleLegStart
				//MiddleLegEnd

				const char* name2 = child2->name();


				if (!isPartOf(name2, "DebugDraw"))
				{
					std::string name2String = name2;
					int listIndex = std::stoi(name2String.substr(name2String.find_first_of("0123456789")));
					if (listIndex > returned.Collision.BoneInfo.size())
					{
						returned.Collision.BoneInfo.push_back(CollisionParam());
					}
					if (isPartOf(name2, std::format("Size{0}",listIndex).c_str()))
					{
						returned.Collision.BoneInfo[listIndex-1].Size = std::stof(child2->value());
					}
					if (isPartOf(name2, std::format("SizeDelta{0}",listIndex).c_str()))
					{
						returned.Collision.BoneInfo[listIndex-1].SizeDelta = std::stof(child2->value());
					}
					for (size_t i = 0; i < collisionBoneNames.size(); i++)
					{
						if (isPartOf(name2, std::format("{0}Start{1}", collisionBoneNames[i], listIndex).c_str()))
						{
							returned.Collision.BoneInfo[listIndex - 1].BoneType = (CollisionBoneType)i;
							returned.Collision.BoneInfo[listIndex - 1].StartFrame = std::stof(child2->value());
						}
						if (isPartOf(name2, std::format("{0}End{1}", collisionBoneNames[i], listIndex).c_str()))
						{
							returned.Collision.BoneInfo[listIndex - 1].EndFrame = std::stof(child2->value());
						}
					}
					
				}			
			}
		}

	}
	returned.MotionMoveSpeedRatio_H.erase(returned.MotionMoveSpeedRatio_H.begin());
	return returned;
}
void RegisterAnims(std::vector<Motion>& vec, rapidxml::xml_node<>* nodeMotion)
{
	/// Fun fact!
	/// Some attacks cannot be converted to the simple evilsonic_attack(name) format, because some have custom
	/// attack names but not custom animation names.
	/// At the same time, not every attack is in the MotionFile
	/// so you have to do both at the same time!!!!
	for (rapidxml::xml_node<>* child = nodeMotion->first_node(); child; child = child->next_sibling())
	{
		auto motionName = child->first_node();
		auto fileName = child->last_node();
		Motion m = Motion();
			m.MotionName = motionName->value();
			m.FileName = fileName->value();
		localMotionFileList.push_back(m);
	}
	for (size_t i = 0; i < vec.size(); i++)
	{
		auto attack = vec[i];
		bool foundInMotionList = false;
		for (size_t i = 0; i < localMotionFileList.size(); i++)
		{
			if (localMotionFileList[i].MotionName == attack.MotionName)
			{
				foundInMotionList = true;
				attack.FileName = localMotionFileList[i].FileName;
				break;
			}
			if (localMotionFileList[i].FileName == attack.MotionName)
			{
				foundInMotionList = true;
				attack.FileName = localMotionFileList[i].FileName;
				break;
			}

		}
		std::string file = attack.FileName;
		if (!foundInMotionList)
		{
			std::string file = std::format("evilsonic_attack{0}", attack.FileName);
			std::string fileU = std::format("Evilsonic_attack{0}", attack.FileName);
			CustomAnimationManager::RegisterAnimation(fileU, file);
			vec[i].FileName = file;
		}
		else
		{
			vec[i].FileName = attack.FileName;
			CustomAnimationManager::RegisterAnimation(file, attack.FileName);
		}
		if (std::find(alreadyRegistered.begin(), alreadyRegistered.end(), attack.MotionName) == alreadyRegistered.end())
		{
			alreadyRegistered.push_back(vec[i].MotionName);
		}
		
	}
}
void RegisterResources(const char* path)
{
	std::string modDir = std::string(path);
	size_t pos = modDir.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		modDir.erase(pos + 1);
	}
	for (size_t i = 0; i < XMLParser::animationTable.size(); i++)
	{
		int attackPos = i;
		/*for (size_t a = 0; a < XMLParser::attacks.size(); a++)
		{
			if (XMLParser::attacks.at(a).MotionName == XMLParser::animationTable.at(i).MotionName)
			{
				attackPos = a;
				break;
			}
		}*/
		std::string filepath = std::format("{0}attack/{1}.tbres.xml", modDir, XMLParser::animationTable.at(i).FileName);
		FILE* file = fopen(filepath.c_str(), "rb");
		if (file == nullptr)
			continue;
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		vector<char> buffer(fileSize);
		fread(buffer.data(), 1, fileSize, file);
		rapidxml::xml_document<> doc;
		buffer.push_back('\0');
		printf(std::format("\nParsing {0}...", XMLParser::animationTable.at(i).FileName).c_str());
		doc.parse<0>(&buffer[0]);
		auto resourceNode = doc.first_node();
		int id = 0;
		//Add trigger functionality too once everything else is done
		for (rapidxml::xml_node<>* TriggerBinderResource_Child = resourceNode->first_node(); TriggerBinderResource_Child; TriggerBinderResource_Child = TriggerBinderResource_Child->next_sibling())
		{
			if (isPartOf(TriggerBinderResource_Child->name(), "ResourceInfo"))
			{
				auto resource = TriggerBinderResource_Child->first_node();
				Resource res;

				for (rapidxml::xml_node<>* ResourceInfo_Child = resource->first_node(); ResourceInfo_Child; ResourceInfo_Child = ResourceInfo_Child->next_sibling())
				{
					auto name = ResourceInfo_Child->name();
					if (isPartOf(name, "ID"))
						res.ID = id;
					if (isPartOf(name, "Type"))
						res.Type = isPartOf(ResourceInfo_Child->value(), "CSB") ? ResourceType::CSB : ResourceType::Effect;
					if (isPartOf(name, "Param"))
					{
						Param p;
						p.FileName = ResourceInfo_Child->first_node()->value();
						if (ResourceInfo_Child->first_node()->next_sibling() != nullptr)
							p.Cue = ResourceInfo_Child->first_node()->next_sibling()->value();
						res.Params = p;
					
					}
				}
				XMLParser::animationTable.at(attackPos).ResourceInfos.Resources.push_back(res);
			}	
			else if (isPartOf(TriggerBinderResource_Child->name(), "TriggerInfo"))
			{
				int indexChildTemp = 0;
				for (rapidxml::xml_node<>* TriggerInfo_Child = TriggerBinderResource_Child->first_node(); TriggerInfo_Child; TriggerInfo_Child = TriggerInfo_Child->next_sibling())
				{
					indexChildTemp++;
					Trigger trig;
					for (rapidxml::xml_node<>* TriggerInfo_Trigger_Child = TriggerInfo_Child->first_node(); TriggerInfo_Trigger_Child; TriggerInfo_Trigger_Child = TriggerInfo_Trigger_Child->next_sibling())
					{
						auto name = TriggerInfo_Trigger_Child->name();
						if (isPartOf(name, "ResourceID"))
						{
							trig.ResourceID = std::stoi(TriggerInfo_Trigger_Child->value());
							continue;
						}
						if (isPartOf(name, "Frame"))
						{
							for (rapidxml::xml_node<>* child3 = TriggerInfo_Trigger_Child->first_node(); child3; child3 = child3->next_sibling())
							{
								auto name2 = child3->name();
								if (isPartOf(name2, "Type"))
								{
									trig.Frame.Type = std::stoi(child3->value());
									continue;
								}
								if (isPartOf(name2, "Start"))
								{
									trig.Frame.Start = std::stoi(child3->value());
									continue;
								}
								if (isPartOf(name2, "End"))
								{
									trig.Frame.End = std::stoi(child3->value());
									continue;
								}
							}
						}
						if (isPartOf(name, "IsFollowNode"))
						{
							trig.IsFollowNode = TriggerInfo_Trigger_Child->value() == "true";
							continue;
						}
						if (isPartOf(name, "IsInheritPositionOnly"))
						{
							trig.IsInheritPositionOnly = TriggerInfo_Trigger_Child->value() == "true";
							continue;
						}
						if (isPartOf(name, "NodeName"))
						{
							trig.NodeName = TriggerInfo_Trigger_Child->value();
							continue;
						}
					}
					XMLParser::animationTable.at(attackPos).TriggerInfos.Resources.push_back(trig);
				}
			}
			id++;			
		}
		fclose(file);
	}
}
void XMLParser::Install(const char* path)
{
	std::ifstream myfile("EvilAttackAction1.xml");
	std::ifstream myfile2("EvilAttackMotionFile.xml");
	rapidxml::xml_document<> doc;
	rapidxml::xml_document<> doc2;

	vector<char> buffer((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
	vector<char> buffer2((std::istreambuf_iterator<char>(myfile2)), std::istreambuf_iterator<char>());

	buffer.push_back('\0');
	buffer2.push_back('\0');


	doc.parse<0>(&buffer[0]);
	doc2.parse<0>(&buffer2[0]);

	auto motionNode = doc.first_node()->first_node()->next_sibling()->next_sibling();
	for (rapidxml::xml_node<>* child = motionNode->first_node(); child; child = child->next_sibling())
	{
		XMLParser::animationTable.push_back(ParseMotionNode(child));
	}
	auto actionNode = doc.first_node()->last_node();
	for (rapidxml::xml_node<>* child = actionNode->first_node(); child; child = child->next_sibling())
	{
		auto attack = ParseActionNode(child, actionNode);
		if (attack.ActionName.find("Start_") != std::string::npos)
			XMLParser::starterAttacks.push_back(attack);
		else
			XMLParser::attacks.push_back(attack);
	}
	auto motionStartpoint = doc2.first_node()->first_node();
	RegisterAnims(XMLParser::animationTable, motionStartpoint);

	RegisterResources(path);
	printf("\nParsing Complete");


}


