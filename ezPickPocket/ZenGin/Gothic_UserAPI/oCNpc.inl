// Supported with union (c) 2020 Union team

// User API for oCNpc
// Add your methods here

int IsAIState_Union(int);
void AI_PlayAni_Union(const zSTRING&);
int GetAivar_Union(const zSTRING& aivar);
void SetAivar_Union(const zSTRING& aivar, int value);
int GetTalent_Union(const zSTRING& talent);
bool GiveItemsTo_Union(oCNpc*, int, int);
bool GiveItemsTo_Union(oCNpc*, const zSTRING&, int);