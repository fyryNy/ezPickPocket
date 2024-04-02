// This file added in headers queue
// File: "Sources.h"
#include "resource.h"

namespace GOTHIC_ENGINE {
		static bool forcetrue = false;
		HOOK Ivk_oCNpc_IsAIState PATCH(&oCNpc::IsAIState, &oCNpc::IsAIState_Union);
		int oCNpc::IsAIState_Union(int State)
		{
				static const int ZS_Talk = parser->GetIndex("ZS_Talk");

				if (forcetrue && State == ZS_Talk)
				{
						return 1;
				}

				return THISCALL(Ivk_oCNpc_IsAIState)(State);
		}

		bool oCNpc::GiveItemsTo_Union(oCNpc* target, int itemInstanz, int itemAmount)
		{
				if (itemInstanz == Invalid)
				{
						return false;
				}

				static auto GiveInvItemsIndex = parser->GetIndex("b_giveinvitems");
				if (GiveInvItemsIndex == Invalid)
				{
						return false;
				}

				static auto GiveInvItemsSymbol = parser->GetSymbol(GiveInvItemsIndex);
				if (!GiveInvItemsSymbol || GiveInvItemsSymbol->type != zPAR_TYPE_FUNC || GiveInvItemsSymbol->ele != 4)
				{
						return false;
				}

				static auto argumentSymbolGiver = []() -> zCPar_Symbol*
						{
								auto symbol = parser->GetSymbol(GiveInvItemsIndex + 1);
								if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
								{
										return nullptr;
								}

								return symbol;
						}();

				static auto argumentSymbolTaker = []() -> zCPar_Symbol*
						{
								auto symbol = parser->GetSymbol(GiveInvItemsIndex + 2);
								if (!symbol || symbol->type != zPAR_TYPE_INSTANCE)
								{
										return nullptr;
								}

								return symbol;
						}();

				static auto argumentSymbolItem = []() -> zCPar_Symbol*
						{
								auto symbol = parser->GetSymbol(GiveInvItemsIndex + 3);
								if (!symbol || symbol->type != zPAR_TYPE_INT)
								{
										return nullptr;
								}

								return symbol;
						}();

				static auto argumentSymbolAmount = []() -> zCPar_Symbol*
						{
								auto symbol = parser->GetSymbol(GiveInvItemsIndex + 4);
								if (!symbol || symbol->type != zPAR_TYPE_INT)
								{
										return nullptr;
								}

								return symbol;
						}();

				if (!argumentSymbolGiver || !argumentSymbolTaker || !argumentSymbolItem || !argumentSymbolAmount)
				{
						return false;
				}

				this->CreateItems(itemInstanz, itemAmount);

				parser->datastack.Clear();

				argumentSymbolGiver->offset = reinterpret_cast<int>(this);
				parser->datastack.Push(GiveInvItemsIndex + 1);

				argumentSymbolTaker->offset = reinterpret_cast<int>(target);
				parser->datastack.Push(GiveInvItemsIndex + 2);

				argumentSymbolItem->SetValue(itemInstanz, 0);
				parser->datastack.Push(itemInstanz);
				parser->datastack.Push(zPAR_TOK_PUSHINT);

				argumentSymbolAmount->SetValue(itemAmount, 0);
				parser->datastack.Push(itemAmount);
				parser->datastack.Push(zPAR_TOK_PUSHINT);

				int pos;
				GiveInvItemsSymbol->GetStackPos(pos, 0);
				parser->DoStack(pos);

				return true;
		}

		bool oCNpc::GiveItemsTo_Union(oCNpc* target, const zSTRING& itemInstanceName, int itemAmount)
		{
				return this->GiveItemsTo_Union(target, parser->GetIndex(itemInstanceName), itemAmount);
		}

		void GivePlayerXP(int XP)
		{
				static auto GiveXPIndex = []() -> int
						{
								if (auto idx = parser->GetIndex("B_GiveXP"); idx != Invalid)
								{
										return idx;
								}

								if (auto idx = parser->GetIndex("B_GivePlayerXP"); idx != Invalid)
								{
										return idx;
								}

								return Invalid;
						}();

						if (GiveXPIndex != Invalid)
						{
								parser->CallFunc(GiveXPIndex, XP);
						}
		}

		bool HasPickPocketDialogue(oCNpc* npc, bool checkcond = false)
		{
				for (auto info : pickpocketInfos)
				{
						if (info->GetNpcID() != npc->GetInstance())
						{
								continue;
						}

						if (checkcond)
						{
								forcetrue = true;

								parser->SetInstance("SELF", npc);
								parser->SetInstance("OTHER", player);

								auto cond = info->InfoConditions();
								forcetrue = false;

								if (!cond)
								{
										return false;
								}

								return true;
						}

						return true;
				}

				return false;
		}

		bool CanPickPocketAmbient(oCNpc* npc)
		{
				if (!PickPocketAmbientNPCs)
				{
						return false;
				}

				if (npc->npcType != NPCTYPE_AMBIENT)
				{
						return false;
				}

				if (npc->guild > NPC_GIL_HUMANS)
				{
						return false;
				}

				if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
				{
						return false;
				}

				if (npc->GetAivar_Union(zSTRING{ "aiv_playerhaspickedmypocket" }))
				{
						return false;
				}

				return true;
		}

		HOOK Ivk_oCGame_UpdatePlayerStatus PATCH(&oCGame::UpdatePlayerStatus, &oCGame::UpdatePlayerStatus_Union);
		void oCGame::UpdatePlayerStatus_Union()
		{
				if (!player || !ogame->GetShowPlayerStatus())
				{
						THISCALL(Ivk_oCGame_UpdatePlayerStatus)();
						return;
				}

				auto focusNpc = player->GetFocusNpc();
				if (!focusNpc || !ShowPickPocketSign || (!CanPickPocketAmbient(focusNpc) && !HasPickPocketDialogue(focusNpc, true)))
				{
						THISCALL(Ivk_oCGame_UpdatePlayerStatus)();
						return;
				}

				zSTRING focusNpcName = focusNpc->name[0];

				focusNpc->name[0] = focusNpcName + zSTRING{ " $" };

				THISCALL(Ivk_oCGame_UpdatePlayerStatus)();

				focusNpc->name[0] = focusNpcName;
		}

		int oCNpc::GetAivar_Union(const zSTRING& aivar)
		{
				auto sym = parser->GetSymbol(aivar);
				if (!sym)
				{
						return Invalid;
				}

				return this->aiscriptvars[sym->single_intdata];
		}

		void oCNpc::SetAivar_Union(const zSTRING& aivar, int value)
		{
				auto sym = parser->GetSymbol(aivar);
				if (!sym)
				{
						return;
				}

				this->aiscriptvars[sym->single_intdata] = value;
		}

		int oCNpc::GetTalent_Union(const zSTRING& talent)
		{
				auto sym = parser->GetSymbol(talent);
				if (!sym)
				{
						return Invalid;
				}

				return this->GetTalentSkill(sym->single_intdata);
		}

		void oCNpc::AI_PlayAni_Union(const zSTRING& anim)
		{
				oCMsgConversation* msgPlayAni = new oCMsgConversation(oCMsgConversation::EV_PLAYANI_NOOVERLAY, anim);
				msgPlayAni->number = this->GetBodyState();
				msgPlayAni->SetHighPriority(True);
				this->GetEM()->OnMessage(msgPlayAni, this);
		}

		void GetPickPockets()
		{
				pickpocketInfos.clear();

				auto list = ogame->GetInfoManager()->infoList.next;
				while (list) {
						auto info = list->data;
						list = list->next;

						if (!info->name.HasWordI("pickpocket") && !info->name.HasWordI("_steal") && !info->name.HasWordI("pickme"))
						{
								continue;
						}

						if (info->name.HasWordI("_DOIT") || info->name.HasWordI("_TRY"))
						{
								continue;
						}

						if (parser->GetIndex(info->name + "_DOIT") == Invalid && parser->GetIndex(info->name + "_TRY") == Invalid)
						{
								continue;
						}

						pickpocketInfos.push_back(info);
				}
		}

		void TryPickPocketAmbient(oCNpc* npc)
		{
				if (!PickPocketAmbientNPCs)
				{
						return;
				}

				if (npc->npcType != 0
#if ENGINE < Engine_G2
						&& npc->npcType != 4
						&& npc->npcType != 6
#else
						&& npc->npcType != 3
						&& npc->npcType != 5
						&& npc->npcType != 6
#endif
						)
				{
						return;
				}

				auto aivar = zSTRING{ "aiv_playerhaspickedmypocket" };

				if (npc->GetAivar_Union(aivar))
				{
						return;
				}

#if ENGINE >= Engine_G2
				auto Currency = oCItemContainer::GetCurrencyInstanceName();
#else
				auto Currency = zSTRING{ "ItMiNugget" };
#endif

				int XP = static_cast<int>(npc->level * 1.75f);

				if (!npc->GiveItemsTo_Union(player, Currency, randomizer.Random(1, XP)))
				{
						return;
				}

				if (GiveAmbientXP)
				{
						GivePlayerXP(XP);
				}

				npc->SetAivar_Union(aivar, 1);

				player->AI_PlayAni_Union(zSTRING{ "T_STEAL" });
		}

		void TryPickPocketDialogue(oCNpc* npc)
		{
				for (auto info : pickpocketInfos)
				{
						if (info->GetNpcID() != npc->GetInstance())
						{
								continue;
						}

						forcetrue = true;

						parser->SetInstance("SELF", npc);
						parser->SetInstance("OTHER", player);

						auto cond = info->InfoConditions();
						forcetrue = false;

						if (!cond)
						{
								return;
						}

						player->AI_PlayAni_Union(zSTRING{ "T_STEAL" });

						{
								auto idx = parser->GetIndex(info->name + "_DOIT");
								if (idx != Invalid)
								{
										parser->CallFunc(idx);
										return;
								}
						}
						{
								auto idx = parser->GetIndex(info->name + "_TRY");
								if (idx != Invalid)
								{

										parser->CallFunc(idx);
										return;
								}
						}
				}
		}

		void TryPickPocket(oCNpc* npc)
		{
				if (player->GetTalent_Union("npc_talent_pickpocket") <= 0)
				{
						return;
				}

				if (npc->guild > NPC_GIL_HUMANS)
				{
						return;
				}

				if (npc->attribute[NPC_ATR_HITPOINTS] <= 0)
				{
						return;
				}

				if (npc->IsUnconscious())
				{
						return;
				}

				if (player->GetDistanceToVob(*npc) > 200.0f)
				{
						return;
				}

				if (ImmersiveMode)
				{
						if (npc->GetWeaponMode() != NPC_WEAPON_NONE)
						{
								return;
						}

						if (player->anictrl->walkmode != ANI_WALKMODE_SNEAK)
						{
								return;
						}

						if (npc->CanSee(player, 0))
						{
								return;
						}
				}

				if (HasPickPocketDialogue(npc))
				{
						TryPickPocketDialogue(npc);
				}
				else
				{
						TryPickPocketAmbient(npc);
				}
		}

		void UpdateSettings()
		{
				ImmersiveMode = zoptions->ReadBool("ezPickPocket", "ImmersiveMode", false);
				PickPocketAmbientNPCs = zoptions->ReadBool("ezPickPocket", "PickPocketAmbientNPCs", false);
				GiveAmbientXP = zoptions->ReadBool("ezPickPocket", "GiveAmbientXP", false);
				ShowPickPocketSign = zoptions->ReadBool("ezPickPocket", "ShowPickPocketSign", false);
		}

		void Game_Entry() {
		}

		void Game_Init() {
				UpdateSettings();
		}

		void Game_Exit() {
		}

		void Game_PreLoop() {
		}

		bool CanBeActive()
		{
				if (!ogame)
				{
						return false;
				}

				if (ogame->singleStep)
				{
						return false;
				}

				if (ogame->pause_screen)
				{
						return false;
				}

				if (!player)
				{
						return false;
				}

				if (player->IsUnconscious())
				{
						return false;
				}

				if (player->attribute[NPC_ATR_HITPOINTS] <= 0)
				{
						return false;
				}

				if (!player->IsHuman())
				{
						return false;
				}

				if (player->inventory2.IsOpen())
				{
						return false;
				}

				if (ogame->GetWorld()->csPlayer->GetPlayingGlobalCutscene())
				{
						return false;
				}

				if (player->fmode != 0)
				{
						return false;
				}

				if (player->sleepingMode != zVOB_AWAKE)
				{
						return false;
				}

				if (player->interactMob || player->interactItem)
				{
						return false;
				}

				if (player->GetBodyState() >= BS_SWIM)
				{
						return false;
				}

				if (player->anictrl && player->anictrl->IsInWeaponChoose())
				{
						return false;
				}

				if (!oCInformationManager::GetInformationManager().HasFinished())
				{
						return false;
				}

				if (!player->GetEM()->IsEmpty(false))
				{
						return false;
				}

				if (zcon->IsVisible())
				{
						return false;
				}

				return true;
		}

		void Game_Loop() {
				if (!CanBeActive())
				{
						return;
				}

				auto focus = player->GetFocusNpc();
				if (!focus)
				{
						return;
				}

				if (zMouseKeyToggled(Right))
				{
						TryPickPocket(focus);
				}
		}

		void Game_PostLoop() {
		}

		void Game_MenuLoop() {
		}

		// Information about current saving or loading world
		TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

		void Game_SaveBegin() {
		}

		void Game_SaveEnd() {
		}

		void LoadBegin() {
		}

		void LoadEnd() {
				GetPickPockets();
		}

		void Game_LoadBegin_NewGame() {
				LoadBegin();
		}

		void Game_LoadEnd_NewGame() {
				LoadEnd();
		}

		void Game_LoadBegin_SaveGame() {
				LoadBegin();
		}

		void Game_LoadEnd_SaveGame() {
				LoadEnd();
		}

		void Game_LoadBegin_ChangeLevel() {
				LoadBegin();
		}

		void Game_LoadEnd_ChangeLevel() {
				LoadEnd();
		}

		void Game_LoadBegin_Trigger() {
		}

		void Game_LoadEnd_Trigger() {
		}

		void Game_Pause() {
		}

		void Game_Unpause() {
		}

		void Game_DefineExternals() {
		}

		void Game_ApplyOptions() {
				UpdateSettings();
		}

		/*
		Functions call order on Game initialization:
				- Game_Entry           * Gothic entry point
				- Game_DefineExternals * Define external script functions
				- Game_Init            * After DAT files init

		Functions call order on Change level:
				- Game_LoadBegin_Trigger     * Entry in trigger
				- Game_LoadEnd_Trigger       *
				- Game_Loop                  * Frame call window
				- Game_LoadBegin_ChangeLevel * Load begin
				- Game_SaveBegin             * Save previous level information
				- Game_SaveEnd               *
				- Game_LoadEnd_ChangeLevel   *

		Functions call order on Save game:
				- Game_Pause     * Open menu
				- Game_Unpause   * Click on save
				- Game_Loop      * Frame call window
				- Game_SaveBegin * Save begin
				- Game_SaveEnd   *

		Functions call order on Load game:
				- Game_Pause              * Open menu
				- Game_Unpause            * Click on load
				- Game_LoadBegin_SaveGame * Load begin
				- Game_LoadEnd_SaveGame   *
		*/

#define AppDefault True
		CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
				Enabled(AppDefault) Game_Entry,
				Enabled(AppDefault) Game_Init,
				Enabled(AppDefault) Game_Exit,
				Enabled(AppDefault) Game_PreLoop,
				Enabled(AppDefault) Game_Loop,
				Enabled(AppDefault) Game_PostLoop,
				Enabled(AppDefault) Game_MenuLoop,
				Enabled(AppDefault) Game_SaveBegin,
				Enabled(AppDefault) Game_SaveEnd,
				Enabled(AppDefault) Game_LoadBegin_NewGame,
				Enabled(AppDefault) Game_LoadEnd_NewGame,
				Enabled(AppDefault) Game_LoadBegin_SaveGame,
				Enabled(AppDefault) Game_LoadEnd_SaveGame,
				Enabled(AppDefault) Game_LoadBegin_ChangeLevel,
				Enabled(AppDefault) Game_LoadEnd_ChangeLevel,
				Enabled(AppDefault) Game_LoadBegin_Trigger,
				Enabled(AppDefault) Game_LoadEnd_Trigger,
				Enabled(AppDefault) Game_Pause,
				Enabled(AppDefault) Game_Unpause,
				Enabled(AppDefault) Game_DefineExternals,
				Enabled(AppDefault) Game_ApplyOptions
		);
}