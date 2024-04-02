# ezPickPocket

This plugin allows to quickly steal from NPCs with a simple click of the right mouse button, without talking to the NPC.
By default it only works with original pickpocket dialogues that are implemented by the game or a mod. It executes all of the functions that would be executed when going into the dialogue and choosing the pickpocket option, so it should be compatible with the original game and also with mods, even if there are changes in game functions (for example it works fine with Chronicles of Myrtana, where the devs modified the pickpocketing system).

Optionally it can be configured to allow pickpocketing of ambient NPCs that don't have pickpocket dialogue (PickPocketAmbientNPCs in gothic.ini).
Enabling this option will generate only gold, but there is a second optional setting that will enable the generation of additional experience (GiveAmbientXP in gothic.ini).

Other optional settings include:
ShowPickPocketSign - enabling this will add a dollar sign ($) to the name of the focused NPC, if they can be pickpocketed
ImmersiveMode - enabling this will add a requirement, so the player has to sneak behind the NPC, to successfully steal from them

Animation yoinked from the New Balance mod. I am not sure if it is an original animation or if it is from another mod.