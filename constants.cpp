#include "stdafx.h"
#include "char.h"
#include "skill.h"

DungeonInfoTable dungeonTable[DUNGEON_TABLE_LENGTH] = {
	{1, 0, 50, 8 , 334, "Ancient jungle", "Great valley of Arelios2" , {4579, 4615}, 1800, 1800, 100, 21, 89, 30862, 4408}, // Index 0
	{2, 0, 40, 8 , 66, "Demon Tower", "Hwang Temple" , {5906, 1108}, 1800, 1800, 95, 21, 89, 0, 1093}, // Index 1
	{1, 1, 75, 8 , 65, "Devil's Catacomb", "Hwang Temple" , {5918, 1003}, 3600, 3600, 85, 21, 89, 30319, 2598}, // Index 2
	{1, 0, 60, 8 , 217, "Spider Queen's Nest", "Spiderdungeon 3" , {689, 6108}, 3600, 1800, 85, 63, 0, 30324, 2092},// Index 3
	{1, 1, 75, 8 , 73, "Dragonroom", "Grotto 2" , {1820, 12205}, 3600, 1800, 105, 63, 36, 30179, 2493}, // Index 4
	{1, 1, 100, 8 , 62, "Red Dragon Fortress", "Fireland" , {5981, 7074}, 3600, 3600, 110, 63, 35, 71095, 6091}, // Index 5
	{1, 1, 100, 8 , 61, "Nemere's Watchtower", "Iceland" , {4322, 1651}, 3600, 3600, 110, 63, 87, 72052, 6191}, // Index 6
	{1, 1, 95, 8 , 405, "Erebos", "Enchanted Wood" , {8283, 14186}, 3600, 1200, 120, 63, 88, 90028, 6418}, // Index 7
	{1, 2, 30, 8 , 62, "Meley's Lair", "Fireland" , {5975, 6992}, 3600, 3600, 105, 63, 35, 0, 6193}, // Index 8
	{1, 1, 110, 8 , 320, "Shipdefense", "Shipbay" , {1689, 6114}, 3600, 3300, 120, 63, 87, 50342, 3960}, // Index 9
	{1, 0, 70, 8 , 334, "Chamber of Wisdom", "Great valley of Arelios2" , {4552, 4671}, 3600, 3300, 90, 63, 0, 30811, 4311}, // Index 10
	{1, 0, 75, 8 , 334, "Forest of Horror", "Great valley of Arelios2" , {4550, 4627}, 3600, 3300, 95, 63, 0, 77453, 236}, // Index 11
	{1, 0, 85, 8 , 334, "Nephrite Cave", "Great valley of Arelios2" , {4552, 4656}, 3600, 3300, 110, 63, 0, 77200, 283}, // Index 12
	{1, 0, 95, 8 , 334, "Deep Shark Cave", "Great valley of Arelios2" , {4552, 4642}, 3600, 3300, 130, 63, 0, 30807, 4307}, // Index 13
	{1, 0, 140, 8 , 334, "Magic Troll Cave", "Great valley of Arelios2" , {4550, 4611}, 3600, 3300, 160, 63, 0, 30781, 4095}, // Index 14
	{1, 0, 150, 8 , 57, "Ancient Pyramid", "Sand of Death" , {7314, 23106}, 3600, 3300, 170, 63, 0, 30799, 4158}, // Index 15
	{1, 0, 180, 8 , 145, "Shadow Tower", "Shadow Enter" , {19084, 22831}, 3600, 3300, 200, 63, 0, 30844, 4388}, // Index 16
	{1, 0, 180, 8 , 334, "Scorpion Ruins ", "Great valley of Arelios2" , {4565, 4615}, 3600, 3300, 200, 63, 0, 30877, 4430} // Index 17
};
#ifdef __AURA_SYSTEM__
int s_aiAuraRefineInfo[AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX] = {
//  [ Grade | LevelMin | LevelMax | NeedEXP | EvolMaterial | EvolMaterialCount | EvolCost | EvolPCT ]
	{1,   1,  49,  1000, 30617, 10,  5000000, 100},
	{2,  50,  99,  2000, 31136, 10,  5000000, 100},
	{3, 100, 149,  4000, 31137, 10,  5000000, 100},
	{4, 150, 199,  8000, 31138, 10,  8000000, 100},
	{5, 200, 249, 16000, 31138, 20, 10000000, 100},
	{6, 250, 250,     0,     0,  0,        0,   0},
	{0,   0,   0,     0,     0,  0,        0,   0}
};

int* GetAuraRefineInfo(BYTE bLevel)
{
	if (bLevel > 250)
		return NULL;

	for (int i = 0; i < AURA_GRADE_MAX_NUM; ++i)
	{
		if (bLevel >= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MIN] && bLevel <= s_aiAuraRefineInfo[i][AURA_REFINE_INFO_LEVEL_MAX])
			return s_aiAuraRefineInfo[i];
	}

	return NULL;
}

int GetAuraRefineInfo(BYTE bGrade, BYTE bInfo)
{
	if (bGrade >= AURA_GRADE_MAX_NUM || bInfo >= AURA_REFINE_INFO_MAX)
		return 0;

	return s_aiAuraRefineInfo[bGrade - 1][bInfo];
}
#endif

TJobInitialPoints JobInitialPoints[JOB_MAX_NUM] = 
/*
   {
   int st, ht, dx, iq;
   int max_hp, max_sp;
   int hp_per_ht, sp_per_iq;
   int hp_per_lv_begin, hp_per_lv_end;
   int sp_per_lv_begin, sp_per_lv_end;
   int max_stamina;
   int stamina_per_con;
   int stamina_per_lv_begin, stamina_per_lv_end;
   }
 */
{
	// str con dex int 초기HP 초기SP  CON/HP INT/SP  HP랜덤/lv   MP랜덤/lv  초기stam  stam/con stam/lv
	{   6,  4,  3,  3,  600,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_WARRIOR  16
	{   4,  3,  6,  3,  650,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_ASSASSIN 16
	{   5,  3,  3,  5,  650,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }, // JOB_SURA	 16
	{   3,  4,  3,  6,  700,   200,     40,    20,    36, 44,     18, 22,     800,      5,      1, 3  }  // JOB_SHAMANa  16
};

const TMobRankStat MobRankStats[MOB_RANK_MAX_NUM] =
/*
   {
   int         iGoldPercent;
   }
 */
{
	{  20,  }, // MOB_RANK_PAWN, 
	{  20,  }, // MOB_RANK_S_PAWN, 
	{  25,  }, // MOB_RANK_KNIGHT, 
	{  30,  }, // MOB_RANK_S_KNIGHT, 
	{  50,  }, // MOB_RANK_BOSS, 
	{ 100,  }  // MOB_RANK_KING,
};

TBattleTypeStat BattleTypeStats[BATTLE_TYPE_MAX_NUM] =
/*
   {
   int         AttGradeBias;
   int         DefGradeBias;
   int         MagicAttGradeBias;
   int         MagicDefGradeBias;
   }
 */
{
	{	  0,	  0,	  0,	-10	}, // BATTLE_TYPE_MELEE, 
	{	 10,	-20,	-10,	-15	}, // BATTLE_TYPE_RANGE,
	{	 -5,	 -5,	 10,	 10	}, // BATTLE_TYPE_MAGIC,  
	{	  0,	  0,	  0,	  0	}, // BATTLE_TYPE_SPECIAL,
	{	 10,	-10,	  0,	-15	}, // BATTLE_TYPE_POWER,  
	{	-10,	 10,	-10,	  0	}, // BATTLE_TYPE_TANKER,
	{	 20,	-20,	  0,	-10	}, // BATTLE_TYPE_SUPER_POWER,
	{	-20,	 20,	-10,	  0	}, // BATTLE_TYPE_SUPER_TANKER,
};

const DWORD * exp_table = NULL;
const DWORD exp_table_common[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,	//	0
	1500,	// Level 1	
	4000,		// Level 2
	7500,		// Level 3	
	12500,		// Level 4	
	21500,		// Level 5	
	36000,		// Level 6	
	55000,		// Level 7	
	85000,		// Level 8	
	120000,		// Level 9	
	165000,	// Level 10 (Angepasst)
	200000,		// Level 11
	220000,		// Level 12
	240000,		// Level 13
	260000,		// Level 14
	280000,		// Level 15
	300000,		// Level 16
	320000,		// Level 17
	340000,		// Level 18
	360000,		// Level 19
	500000,	// Level 20 (Angepasst)
	600000,		// Level 21
	700000,		// Level 22
	800000,		// Level 23
	900000,		// Level 24
	1000000,	// Level 25	
	1100000,	// Level 26	
	1200000,	// Level 27	
	1300000,	// Level 28	
	1400000,	// Level 29	
	1500000,	// Level 30 (Angepasst)
	2000000,		// Level 31
	2300000,		// Level 32
	2600000,		// Level 33
	2900000,		// Level 34
	3200000,		// Level 35
	3500000,		// Level 36
	3800000,		// Level 37
	4100000,		// Level 38
	4400000,		// Level 39
	5000000,	// Level 40 (Angepasst)
	7000000,		// Level 41
	7300000,		// Level 42
	7600000,		// Level 43
	7900000,		// Level 44
	8100000,		// Level 45
	8400000,		// Level 46
	8700000,		// Level 47
	9000000,		// Level 48
	9500000,		// Level 49
	10000000,	// Level 50 (Angepasst)
	10750000,		// Level 51
	11500000,		// Level 52
	12300000,		// Level 53
	13050000,		// Level 54
	13800000,		// Level 55
	15000000,		// Level 56
	16000000,		// Level 57
	17000000,		// Level 58
	18000000,		// Level 59
	20000000,	// Level 60
	25000000,		// Level 61
	30000000,		// Level 62
	35000000,		// Level 63
	40000000,		// Level 64
	45000000,		// Level 65
	50000000,		// Level 66
	55000000,		// Level 67
	60000000,		// Level 68
	65000000,		// Level 69
	70000000,	// Level 70
	78000000,		// Level 71
	86000000,		// Level 72
	94000000,		// Level 73
	98000000,		// Level 74
	110000000,		// Level 75
	115000000,		// Level 76
	117000000,		// Level 77
	119000000,		// Level 78
	120000000,		// Level 79
	125000000,	// Level 80
	130000000,		// Level 81
	140000000,		// Level 82
	160000000,		// Level 83
	180000000,		// Level 84
	200000000,		// Level 85
	220000000,		// Level 86
	240000000,		// Level 87
	260000000,		// Level 88
	280000000,		// Level 89
	300000000,	// Level 90
	500000000,		// Level 91
	700000000,		// Level 92
	900000000,		// Level 93
	1100000000,		// Level 94
	1300000000,		// Level 95
	1500000000,		// Level 96
	1700000000,		// Level 97
	1900000000,		// Level 98
	2100000000,	// Level 99
	2300000000,	// Level 100
	2350000000,		// Level 101
	2400000000,		// Level 102
	2450000000,		// Level 103
	2500000000,		// Level 104
	2550000000,	// Level 105
	2600000000,		// Level 106
	2650000000,		// Level 107
	2700000000,		// Level 108
	2750000000,		// Level 109
	2800000000,	// Level 110
	2900000000,		// Level 111
	2950000000,		// Level 112
	3000000000,		// Level 113
	3050000000,		// Level 114
	3100000000,	// Level 115
	3150000000,		// Level 116
	3200000000,		// Level 117
	3250000000,		// Level 118
	3300000000,		// Level 119
	0,	// Level 120 Elendos V2 ENDE
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 130
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 140
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 150
	3390000000,
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 160
	3390000000,
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 170
	3390000000,
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	
	3390000000,	 // 180
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 
	3390000000,	 // 190
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000, // 200
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000, // 210
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000, // 220
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000, // 230
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000, // 240
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3390000000,
	3399999990, // 250
};

const int * aiPercentByDeltaLev = NULL;
const int * aiPercentByDeltaLevForBoss = NULL;

// 적과 나와의 레벨차이에 의한 계산에 사용되는 테이블
// MIN(MAX_EXP_DELTA_OF_LEV - 1, (적렙 + 15) - 내렙))
const int aiPercentByDeltaLevForBoss_euckr[MAX_EXP_DELTA_OF_LEV] =
{
	1,      // -15  0
	3,          // -14  1
	5,          // -13  2
	7,          // -12  3
	15,         // -11  4
	30,         // -10  5
	60,         // -9   6
	90,         // -8   7
	91,         // -7   8
	92,         // -6   9
	93,         // -5   10
	94,         // -4   11
	95,         // -3   12
	97,         // -2   13
	99,         // -1   14
	100,        // 0    15
	105,        // 1    16
	110,        // 2    17
	115,        // 3    18
	120,        // 4    19
	125,        // 5    20
	130,        // 6    21
	135,        // 7    22
	140,        // 8    23
	145,        // 9    24
	150,        // 10   25
	155,        // 11   26
	160,        // 12   27
	165,        // 13   28
	170,        // 14   29
	180         // 15   30
};



const int aiPercentByDeltaLev_euckr[MAX_EXP_DELTA_OF_LEV] =
{
	1,  //  -15 0
	5,  //  -14 1
	10, //  -13 2
	20, //  -12 3
	30, //  -11 4
	50, //  -10 5
	70, //  -9  6
	80, //  -8  7
	85, //  -7  8
	90, //  -6  9
	92, //  -5  10
	94, //  -4  11
	96, //  -3  12
	98, //  -2  13
	100,    //  -1  14
	100,    //  0   15
	105,    //  1   16
	110,    //  2   17
	115,    //  3   18
	120,    //  4   19
	125,    //  5   20
	130,    //  6   21
	135,    //  7   22
	140,    //  8   23
	145,    //  9   24
	150,    //  10  25
	155,    //  11  26
	160,    //  12  27
	165,    //  13  28
	170,    //  14  29
	180,    //  15  30
};

const DWORD party_exp_distribute_table[PLAYER_MAX_LEVEL_CONST + 1] = 
{
	0,
	10,		10,		10,		10,		15,		15,		20,		25,		30,		40,		// 1 - 10
	50,		60,		80,		100,	120,	140,	160,	184,	210,	240,	// 11 - 20
	270,	300,	330,	360,	390,	420,	450,	480,	510,	550,	// 21 - 30
	600,	640,	700,	760,	820,	880,	940,	1000,	1100,	1180,	// 31 - 40
	1260,	1320,	1380,	1440,	1500,	1560,	1620,	1680,	1740,	1800,	// 41 - 50
	1860,	1920,	2000,	2100,	2200,	2300,	2450,	2600,	2750,	2900,	// 51 - 60
	3050,	3200,	3350,	3500,	3650,	3800,	3950,	4100,	4250,	4400,	// 61 - 70
	4600,	4800,	5000,	5200,	5400,	5600,	5800,	6000,	6200,	6400,	// 71 - 80
	6600,	6900,	7100,	7300,	7600,	7800,	8000,	8300,	8500,	8800,	// 81 - 90
	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	9000,	// 91 - 100
	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	10000,	// 101 - 110
	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	12000,	// 111 - 120
};

Coord aArroundCoords[ARROUND_COORD_MAX_NUM] = 
{
	{	     0,	      0	    },
	{        0,      50     },
	{       35,     35      },
	{       50,     -0      },
	{       35,     -35     },
	{       0,      -50     },
	{       -35,    -35     },
	{       -50,    0       },
	{       -35,    35      },
	{       0,      100     },
	{       71,     71      },
	{       100,    -0      },
	{       71,     -71     },
	{       0,      -100    },
	{       -71,    -71     },
	{       -100,   0       },
	{       -71,    71      },
	{       0,      150     },
	{       106,    106     },
	{       150,    -0      },
	{       106,    -106    },
	{       0,      -150    },
	{       -106,   -106    },
	{       -150,   0       },
	{       -106,   106     },
	{       0,      200     },
	{       141,    141     },
	{       200,    -0      },
	{       141,    -141    },
	{       0,      -200    },
	{       -141,   -141    },
	{       -200,   0       },
	{       -141,   141     },
	{       0,      250     },
	{       177,    177     },
	{       250,    -0      },
	{       177,    -177    },
	{       0,      -250    },
	{       -177,   -177    },
	{       -250,   0       },
	{       -177,   177     },
	{       0,      300     },
	{       212,    212     },
	{       300,    -0      },
	{       212,    -212    },
	{       0,      -300    },
	{       -212,   -212    },
	{       -300,   0       },
	{       -212,   212     },
	{       0,      350     },
	{       247,    247     },
	{       350,    -0      },
	{       247,    -247    },
	{       0,      -350    },
	{       -247,   -247    },
	{       -350,   0       },
	{       -247,   247     },
	{       0,      400     },
	{       283,    283     },
	{       400,    -0      },
	{       283,    -283    },
	{       0,      -400    },
	{       -283,   -283    },
	{       -400,   0       },
	{       -283,   283     },
	{       0,      450     },
	{       318,    318     },
	{       450,    -0      },
	{       318,    -318    },
	{       0,      -450    },
	{       -318,   -318    },
	{       -450,   0       },
	{       -318,   318     },
	{       0,      500     },
	{       354,    354     },
	{       500,    -0      },
	{       354,    -354    },
	{       0,      -500    },
	{       -354,   -354    },
	{       -500,   0       },
	{       -354,   354     },
	{       0,      550     },
	{       389,    389     },
	{       550,    -0      },
	{       389,    -389    },
	{       0,      -550    },
	{       -389,   -389    },
	{       -550,   0       },
	{       -389,   389     },
	{       0,      600     },
	{       424,    424     },
	{       600,    -0      },
	{       424,    -424    },
	{       0,      -600    },
	{       -424,   -424    },
	{       -600,   0       },
	{       -424,   424     },
	{       0,      650     },
	{       460,    460     },
	{       650,    -0      },
	{       460,    -460    },
	{       0,      -650    },
	{       -460,   -460    },
	{       -650,   0       },
	{       -460,   460     },
	{       0,      700     },
	{       495,    495     },
	{       700,    -0      },
	{       495,    -495    },
	{       0,      -700    },
	{       -495,   -495    },
	{       -700,   0       },
	{       -495,   495     },
	{       0,      750     },
	{       530,    530     },
	{       750,    -0      },
	{       530,    -530    },
	{       0,      -750    },
	{       -530,   -530    },
	{       -750,   0       },
	{       -530,   530     },
	{       0,      800     },
	{       566,    566     },
	{       800,    -0      },
	{       566,    -566    },
	{       0,      -800    },
	{       -566,   -566    },
	{       -800,   0       },
	{       -566,   566     },
	{       0,      850     },
	{       601,    601     },
	{       850,    -0      },
	{       601,    -601    },
	{       0,      -850    },
	{       -601,   -601    },
	{       -850,   0       },
	{       -601,   601     },
	{       0,      900     },
	{       636,    636     },
	{       900,    -0      },
	{       636,    -636    },
	{       0,      -900    },
	{       -636,   -636    },
	{       -900,   0       },
	{       -636,   636     },
	{       0,      950     },
	{       672,    672     },
	{       950,    -0      },
	{       672,    -672    },
	{       0,      -950    },
	{       -672,   -672    },
	{       -950,   0       },
	{       -672,   672     },
	{       0,      1000    },
	{       707,    707     },
	{       1000,   -0      },
	{       707,    -707    },
	{       0,      -1000   },
	{       -707,   -707    },
	{       -1000,  0       },
	{       -707,   707     },
};

const DWORD guild_exp_table[GUILD_MAX_LEVEL+1] =
{
	0,
	15000UL,
	45000UL,
	90000UL,
	160000UL,
	235000UL,
	325000UL,
	430000UL,
	550000UL,
	685000UL,
	835000UL,
	1000000UL,
	1500000UL,
	2100000UL,
	2800000UL,
	3600000UL,
	4500000UL,
	6500000UL,
	8000000UL,
	10000000UL,
	42000000UL
};

// INTERNATIONAL_VERSION 길드경험치
const DWORD guild_exp_table2[GUILD_MAX_LEVEL+1] =
{
	0,	
	6000UL,
	18000UL,
	36000UL,
	64000UL,
	94000UL,
	130000UL,
	172000UL,
	220000UL,
	274000UL,
	334000UL,
	400000UL,
	600000UL,
	840000UL,
	1120000UL,
	1440000UL,
	1800000UL,
	2600000UL,
	3200000UL,
	4000000UL,
	16800000UL
};
// END_OF_INTERNATIONAL_VERSION 길드경험치

const int aiMobEnchantApplyIdx[MOB_ENCHANTS_MAX_NUM] =
{
	APPLY_CURSE_PCT,
	APPLY_SLOW_PCT,
	APPLY_POISON_PCT,
	APPLY_STUN_PCT,
	APPLY_CRITICAL_PCT,
	APPLY_PENETRATE_PCT,
};

const int aiMobResistsApplyIdx[MOB_RESISTS_MAX_NUM] =
{
	APPLY_RESIST_SWORD,
	APPLY_RESIST_TWOHAND,
	APPLY_RESIST_DAGGER,
	APPLY_RESIST_BELL,
	APPLY_RESIST_FAN,
	APPLY_RESIST_BOW,
	APPLY_RESIST_FIRE,
	APPLY_RESIST_ELEC,
	APPLY_RESIST_MAGIC,
	APPLY_RESIST_WIND,
	APPLY_POISON_REDUCE,
};

const int aiSocketPercentByQty[5][4] =
{
	{  0,  0,  0,  0 },
	{  3,  0,  0,  0 },
	{ 10,  1,  0,  0 },
	{ 15, 10,  1,  0 },
	{ 20, 15, 10,  1 }
};

const int aiWeaponSocketQty[WEAPON_NUM_TYPES] =
{
#ifdef ENABLE_EXTENDED_SOCKETS
	6, // WEAPON_SWORD,
	6, // WEAPON_DAGGER,
	6, // WEAPON_BOW,
	6, // WEAPON_TWO_HANDED,
	6, // WEAPON_BELL,
	6, // WEAPON_FAN,
#else
	3, // WEAPON_SWORD,
	3, // WEAPON_DAGGER,
	3, // WEAPON_BOW,
	3, // WEAPON_TWO_HANDED,
	3, // WEAPON_BELL,
	3, // WEAPON_FAN,
#endif
	0, // WEAPON_ARROW,
	0, // WEAPON_MOUNT_SPEAR
};

const int aiArmorSocketQty[ARMOR_NUM_TYPES] =
{
#ifdef ENABLE_EXTENDED_SOCKETS
	6, // ARMOR_BODY,
#else
	3, // ARMOR_BODY,
#endif
	1, // ARMOR_HEAD,
	1, // ARMOR_SHIELD,
	0, // ARMOR_WRIST,
	0, // ARMOR_FOOTS,
	0  // ARMOR_ACCESSORY
};

TItemAttrMap g_map_itemAttr;
TItemAttrMap g_map_itemRare;

const TApplyInfo aApplyInfo[MAX_APPLY_NUM] =
/*
{
   DWORD dwPointType;
}
 */
{
	// Point Type
	{ POINT_NONE,			},   // APPLY_NONE,		0
	{ POINT_MAX_HP,		        },   // APPLY_MAX_HP,		1
	{ POINT_MAX_SP,		        },   // APPLY_MAX_SP,		2
	{ POINT_HT,			        },   // APPLY_CON,		3
	{ POINT_IQ,			        },   // APPLY_INT,		4
	{ POINT_ST,			        },   // APPLY_STR,		5
	{ POINT_DX,			        },   // APPLY_DEX,		6
	{ POINT_ATT_SPEED,		        },   // APPLY_ATT_SPEED,	7
	{ POINT_MOV_SPEED,		        },   // APPLY_MOV_SPEED,	8
	{ POINT_CASTING_SPEED,	        },   // APPLY_CAST_SPEED,	9
	{ POINT_HP_REGEN,			},   // APPLY_HP_REGEN,		10
	{ POINT_SP_REGEN,			},   // APPLY_SP_REGEN,		11
	{ POINT_POISON_PCT,		        },   // APPLY_POISON_PCT,	12
	{ POINT_STUN_PCT,		        },   // APPLY_STUN_PCT,		13
	{ POINT_SLOW_PCT,		        },   // APPLY_SLOW_PCT,		14
	{ POINT_CRITICAL_PCT,		},   // APPLY_CRITICAL_PCT,	15
	{ POINT_PENETRATE_PCT,	        },   // APPLY_PENETRATE_PCT,	16
	{ POINT_ATTBONUS_HUMAN,	        },   // APPLY_ATTBONUS_HUMAN,	17
	{ POINT_ATTBONUS_ANIMAL,	        },   // APPLY_ATTBONUS_ANIMAL,	18
	{ POINT_ATTBONUS_ORC,		},   // APPLY_ATTBONUS_ORC,	19
	{ POINT_ATTBONUS_MILGYO,	        },   // APPLY_ATTBONUS_MILGYO,	20
	{ POINT_ATTBONUS_UNDEAD,	        },   // APPLY_ATTBONUS_UNDEAD,	21
	{ POINT_ATTBONUS_DEVIL,	        },   // APPLY_ATTBONUS_DEVIL,	22
	{ POINT_STEAL_HP,		        },   // APPLY_STEAL_HP,		23
	{ POINT_STEAL_SP,		        },   // APPLY_STEAL_SP,		24
	{ POINT_MANA_BURN_PCT,	        },   // APPLY_MANA_BURN_PCT,	25
	{ POINT_DAMAGE_SP_RECOVER,	        },   // APPLY_DAMAGE_SP_RECOVER,26
	{ POINT_BLOCK,		        },   // APPLY_BLOCK,		27
	{ POINT_DODGE,		        },   // APPLY_DODGE,		28
	{ POINT_RESIST_SWORD,		},   // APPLY_RESIST_SWORD,	29
	{ POINT_RESIST_TWOHAND,	        },   // APPLY_RESIST_TWOHAND,	30
	{ POINT_RESIST_DAGGER,	        },   // APPLY_RESIST_DAGGER,	31
	{ POINT_RESIST_BELL,		},   // APPLY_RESIST_BELL,	32
	{ POINT_RESIST_FAN,		        },   // APPLY_RESIST_FAN,	33
	{ POINT_RESIST_BOW,		        },   // APPLY_RESIST_BOW,	34
	{ POINT_RESIST_FIRE,		},   // APPLY_RESIST_FIRE,	35
	{ POINT_RESIST_ELEC,		},   // APPLY_RESIST_ELEC,	36
	{ POINT_RESIST_MAGIC,		},   // APPLY_RESIST_MAGIC,	37
	{ POINT_RESIST_WIND,		},   // APPLY_RESIST_WIND,	38
	{ POINT_REFLECT_MELEE,	        },   // APPLY_REFLECT_MELEE,	39
	{ POINT_REFLECT_CURSE,	        },   // APPLY_REFLECT_CURSE,	40
	{ POINT_POISON_REDUCE,	        },   // APPLY_POISON_REDUCE,	41
	{ POINT_KILL_SP_RECOVER,	        },   // APPLY_KILL_SP_RECOVER,	42
	{ POINT_EXP_DOUBLE_BONUS,	        },   // APPLY_EXP_DOUBLE_BONUS,	43
	{ POINT_GOLD_DOUBLE_BONUS,	        },   // APPLY_GOLD_DOUBLE_BONUS,44
	{ POINT_ITEM_DROP_BONUS,	        },   // APPLY_ITEM_DROP_BONUS,	45
	{ POINT_POTION_BONUS,		},   // APPLY_POTION_BONUS,	46
	{ POINT_KILL_HP_RECOVERY,	        },   // APPLY_KILL_HP_RECOVER,	47
	{ POINT_IMMUNE_STUN,		},   // APPLY_IMMUNE_STUN,	48
	{ POINT_IMMUNE_SLOW,		},   // APPLY_IMMUNE_SLOW,	49
	{ POINT_IMMUNE_FALL,		},   // APPLY_IMMUNE_FALL,	50
	{ POINT_NONE,			},   // APPLY_SKILL,		51
	{ POINT_BOW_DISTANCE,		},   // APPLY_BOW_DISTANCE,	52
	{ POINT_ATT_GRADE_BONUS,	        },   // APPLY_ATT_GRADE,	53
	{ POINT_DEF_GRADE_BONUS,	        },   // APPLY_DEF_GRADE,	54
	{ POINT_MAGIC_ATT_GRADE_BONUS,      },   // APPLY_MAGIC_ATT_GRADE,	55
	{ POINT_MAGIC_DEF_GRADE_BONUS,      },   // APPLY_MAGIC_DEF_GRADE,	56
	{ POINT_CURSE_PCT,			},   // APPLY_CURSE_PCT,	57
	{ POINT_MAX_STAMINA			},   // APPLY_MAX_STAMINA	58
	{ POINT_ATTBONUS_WARRIOR		},   // APPLY_ATTBONUS_WARRIOR  59
	{ POINT_ATTBONUS_ASSASSIN		},   // APPLY_ATTBONUS_ASSASSIN 60
	{ POINT_ATTBONUS_SURA		},   // APPLY_ATTBONUS_SURA     61
	{ POINT_ATTBONUS_SHAMAN		},   // APPLY_ATTBONUS_SHAMAN   62
	{ POINT_ATTBONUS_MONSTER		},   //	APPLY_ATTBONUS_MONSTER  63
	{ POINT_ATT_BONUS		},   // 64 // APPLY_MALL_ATTBONUS
	{ POINT_MALL_DEFBONUS		},   // 65
	{ POINT_MALL_EXPBONUS		},   // 66 APPLY_MALL_EXPBONUS
	{ POINT_MALL_ITEMBONUS		},   // 67
	{ POINT_MALL_GOLDBONUS		},   // 68
	{ POINT_MAX_HP_PCT			},		// 69
	{ POINT_MAX_SP_PCT			},		// 70
	{ POINT_SKILL_DAMAGE_BONUS		},	// 71
	{ POINT_NORMAL_HIT_DAMAGE_BONUS	},	// 72

	// DEFEND_BONUS_ATTRIBUTES
	{ POINT_SKILL_DEFEND_BONUS		},	// 73
	{ POINT_NORMAL_HIT_DEFEND_BONUS	},	// 74
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	// PC_BANG_ITEM_ADD
	{ POINT_PC_BANG_EXP_BONUS	},		// 75
	{ POINT_PC_BANG_DROP_BONUS	},		// 76
	// END_PC_BANG_ITEM_ADD

	{ POINT_NONE,		},				// 77 사용시 HP 소모 APPLY_EXTRACT_HP_PCT

	{ POINT_RESIST_WARRIOR,		},		// 78 무사에게 저항 APPLY_RESIST_WARRIOR
	{ POINT_RESIST_ASSASSIN,	},		// 79 자객에게 저항 APPLY_RESIST_ASSASSIN
	{ POINT_RESIST_SURA,		},		// 80 수라에게 저항 APPLY_RESIST_SURA
	{ POINT_RESIST_SHAMAN,		},		// 81 무당에게 저항 APPLY_RESIST_SHAMAN
	{ POINT_ENERGY				},		// 82 기력 
	{ POINT_DEF_GRADE			},		// 83 방어력. DEF_GRADE_BONUS는 클라에서 두배로 보여지는 의도된 버그(...)가 있다.
	{ POINT_COSTUME_ATTR_BONUS	},		// 84 코스튬에 붙은 속성에 대해서만 보너스를 주는 기력
	{ POINT_MAGIC_ATT_BONUS_PER },			// 85 마법 공격력 +x%
	{ POINT_MELEE_MAGIC_ATT_BONUS_PER		},			// 86 APPLY_MELEE_MAGIC_ATTBONUS_PER
	{ POINT_RESIST_ICE,			},   // APPLY_RESIST_ICE,	87
	{ POINT_RESIST_EARTH,		},   // APPLY_RESIST_EARTH,	88
	{ POINT_RESIST_DARK,		},   // APPLY_RESIST_DARK,	89
	{ POINT_RESIST_CRITICAL,		},   // APPLY_ANTI_CRITICAL_PCT,	90
	{ POINT_RESIST_PENETRATE,		},   // APPLY_ANTI_PENETRATE_PCT,	91
#ifdef ELEMENT_NEW_BONUSES
	{ POINT_ATTBONUS_ELEC,},
	{ POINT_ATTBONUS_FIRE, },
	{ POINT_ATTBONUS_ICE, },
	{ POINT_ATTBONUS_WIND, },
	{ POINT_ATTBONUS_EARTH, },
	{ POINT_ATTBONUS_DARK, },
#endif
#ifdef ENABLE_SHADOW_BONUS
	{ POINT_ATTBONUS_SHADOW, }, 
#else
	{ POINT_NONE,			},
#endif
#ifdef __NEW_ATTRIBUTES__
	{ POINT_ATTBONUS_METIN, 					},	// 99
	{ POINT_ATTBONUS_BOSS,  					},	// 100
#else
	{ POINT_NONE, 								},	// 99
	{ POINT_NONE,  								},	// 100
#endif	
};

const int aiItemMagicAttributePercentHigh[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	//25, 25, 40, 8, 2,
	30, 40, 20, 8, 2
};

const int aiItemMagicAttributePercentLow[ITEM_ATTRIBUTE_MAX_LEVEL] =
{
	//45, 25, 20, 10, 0,
	50, 40, 10, 0, 0
};

// ADD_ITEM_ATTRIBUTE
const int aiItemAttributeAddPercent[ITEM_ATTRIBUTE_MAX_NUM] =
{
	100, 80, 60, 50, 30, 0, 0, 
};
// END_OF_ADD_ITEM_ATTRIBUTE

const int aiExpLossPercents[PLAYER_EXP_TABLE_MAX + 1] =
{
	0,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 4, // 1 - 10
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, // 11 - 20
	4, 4, 4, 4, 4, 4, 4, 3, 3, 3, // 21 - 30
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // 31 - 40
	3, 3, 3, 3, 2, 2, 2, 2, 2, 2, // 41 - 50
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // 51 - 60
	2, 2, 1, 1, 1, 1, 1, 1, 1, 1, // 61 - 70
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 71 - 80
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 81 - 90
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 91 - 100
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 101 - 110
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 111 - 120
};

const int aiSkillBookCountForLevelUp[10] =
{
	3, 3, 3, 3, 3, 4, 4, 5, 5, 6
};

// ADD_GRANDMASTER_SKILL
const int aiGrandMasterSkillBookCountForLevelUp[10] =
{
	3, 3, 5, 5, 7, 7, 10, 10, 10, 20, 
};

const int aiGrandMasterSkillBookMinCount[10] = 
{
//	1, 1, 3, 5, 10, 15, 20, 30, 40, 50, 
//	3, 3, 5, 5, 10, 10, 15, 15, 20, 30
	1, 1, 1, 2,  2,  3,  3,  4,  5,  6
};

const int aiGrandMasterSkillBookMaxCount[10] = 
{
//	6, 15, 30, 45, 60, 80, 100, 120, 160, 200,
//	6, 10, 15, 20, 30, 40, 50, 60, 70, 80
	5,  7,  9, 11, 13, 15, 20, 25, 30, 35
};
// END_OF_ADD_GRANDMASTER_SKILL

const int CHN_aiPartyBonusExpPercentByMemberCount[9] =
{
	0, 0, 12, 18, 26, 40, 53, 70, 100
};


// UPGRADE_PARTY_BONUS
const int KOR_aiPartyBonusExpPercentByMemberCount[9] =
{
	0, 
	0, 
	30, // 66% * 2 - 100  
	60, // 53% * 3 - 100 
	75, // 44% * 4 - 100
	90, // 38% * 5 - 100
	105, // 34% * 6 - 100
	110, // 30% * 7 - 100
	140, // 30% * 8 - 100
};

const int KOR_aiUniqueItemPartyBonusExpPercentByMemberCount[9] =
{
	0, 
	0, 
	15*2, 
	14*3, 
	13*4, 
	12*5, 
	11*6, 
	10*7, 
	10*8,
};
// END_OF_UPGRADE_PARTY_BONUS

const int * aiChainLightningCountBySkillLevel = NULL;

const int aiChainLightningCountBySkillLevel_euckr[SKILL_MAX_LEVEL+1] =
{
	0,	// 0
	2,	// 1
	2,	// 2
	2,	// 3
	2,	// 4
	2,	// 5
	2,	// 6
	2,	// 7
	2,	// 8
	3,	// 9
	3,	// 10
	3,	// 11
	3,	// 12
	3,	// 13
	3,	// 14
	3,	// 15
	3,	// 16
	3,	// 17
	3,	// 18
	4,	// 19
	4,	// 20
	4,	// 21
	4,	// 22
	4,	// 23
	5,	// 24
	5,	// 25
	5,	// 26
	5,	// 27
	5,	// 28
	5,	// 29
	5,	// 30
	5,	// 31
	5,	// 32
	5,	// 33
	5,	// 34
	5,	// 35
	5,	// 36
	5,	// 37
	5,	// 38
	5,	// 39
	5,	// 40
};

const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM] =
{
	//  mob		pct	{+0	+1	+2	+3	+4}
	{8005,	60,	{30,	30,	30,	9,	1}	},
	{8006,	60,	{28,	29,	31,	11,	1}	},
	{8007,	60,	{24,	29,	32,	13,	2}	},
	{8008,	60,	{22,	28,	33,	15,	2}	},
	{8009,	60,	{21,	27,	33,	17,	2}	},
	{8010,	60,	{18,	26,	34,	20,	2}	},
	{8011,	60,	{14,	26,	35,	22,	3}	},
	{8012,	60,	{10,	26,	37,	24,	3}	},
	{8013,	60,	{2,	26,	40,	29,	3}	},
	{8014,	60,	{0,	26,	41,	30,	3}	},
};

const char * c_apszEmpireNames[EMPIRE_MAX_NUM] =
{
	"전제국",
	"신수국",
	"천조국",
	"진노국"
};

const char * c_apszPrivNames[MAX_PRIV_NUM] =
{
	"",
	"아이템이 나올 확률",
	"돈이 나올 확률",
	"돈 대박이 나올 확률",
	"경험치 배율",
};

const int aiPolymorphPowerByLevel[SKILL_MAX_LEVEL + 1] =
{
	10,   // 1
	11,   // 2
	11,   // 3
	12,   // 4
	13,   // 5
	13,   // 6
	14,   // 7
	15,   // 8
	16,   // 9
	17,   // 10
	18,   // 11
	19,   // 12
	20,   // 13
	22,   // 14
	23,   // 15
	24,   // 16
	26,   // 17
	27,   // 18
	29,   // 19
	31,   // 20
	33,   // 21
	35,   // 22
	37,   // 23
	39,   // 24
	41,   // 25
	44,   // 26
	46,   // 27
	49,   // 28
	52,   // 29
	55,   // 30
	59,   // 31
	62,   // 32
	66,   // 33
	70,   // 34
	74,   // 35
	79,   // 36
	84,   // 37
	89,   // 38
	94,   // 39
	100,  // 40
};

TGuildWarInfo KOR_aGuildWarInfo[GUILD_WAR_TYPE_MAX_NUM] =
/*
   {
   long lMapIndex;
   int iWarPrice;
   int iWinnerPotionRewardPctToWinner;
   int iLoserPotionRewardPctToWinner;
   int iInitialScore;
   int iEndScore;
   };
 */
{
	{ 0,        0,      0,      0,      0,      0       },
	{ 110,      0,      100,    50,     0,      100     },
	{ 111,      0,      100,    50,     0,      10      },
};

//
// 악세서리 소켓용 수치들
//

// 다이아몬드로 소켓을 추가할 때 확률
const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM] =
{
	50, 50, 50
};

// 악세서리 수치 값의 몇%만큼의 성능을 추가하는지
const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] = 
{
	0, 10, 20, 40
};

// 소켓 지속시간 24, 12, 6
const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	0, 3600 * 24, 3600 * 12, 3600 * 6
};

// 소켓 장착 성공률
const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1] =
{
	90, 80, 70, 0
};
// END_OF_ACCESSORY_REFINE

#include "../../common/length.h"
// from import_item_proto.c
typedef struct SValueName
{
    const char *	c_pszName;
    long		lValue;
} TValueName;

TValueName c_aApplyTypeNames[] =
{
    { "STR",		APPLY_STR		},
    { "DEX",		APPLY_DEX		},
    { "CON",		APPLY_CON		},
    { "INT",		APPLY_INT		},
    { "MAX_HP",		APPLY_MAX_HP		},
    { "MAX_SP",		APPLY_MAX_SP		},
    { "MAX_STAMINA",	APPLY_MAX_STAMINA	},
    { "POISON_REDUCE",	APPLY_POISON_REDUCE	},
    { "EXP_DOUBLE_BONUS", APPLY_EXP_DOUBLE_BONUS },
    { "GOLD_DOUBLE_BONUS", APPLY_GOLD_DOUBLE_BONUS },
    { "ITEM_DROP_BONUS", APPLY_ITEM_DROP_BONUS	},
    { "HP_REGEN",	APPLY_HP_REGEN		},
    { "SP_REGEN",	APPLY_SP_REGEN		},
    { "ATTACK_SPEED",	APPLY_ATT_SPEED		},
    { "MOVE_SPEED",	APPLY_MOV_SPEED		},
    { "CAST_SPEED",	APPLY_CAST_SPEED	},
    { "ATT_BONUS",	APPLY_ATT_GRADE_BONUS	},
    { "DEF_BONUS",	APPLY_DEF_GRADE_BONUS	},
    { "MAGIC_ATT_GRADE",APPLY_MAGIC_ATT_GRADE	},
    { "MAGIC_DEF_GRADE",APPLY_MAGIC_DEF_GRADE	},
    { "SKILL",		APPLY_SKILL		},
    { "ATTBONUS_ANIMAL",APPLY_ATTBONUS_ANIMAL	},
    { "ATTBONUS_UNDEAD",APPLY_ATTBONUS_UNDEAD	},
    { "ATTBONUS_DEVIL", APPLY_ATTBONUS_DEVIL	},
    { "ATTBONUS_HUMAN", APPLY_ATTBONUS_HUMAN	},
    { "ADD_BOW_DISTANCE",APPLY_BOW_DISTANCE	},
    { "DODGE",		APPLY_DODGE		},
    { "BLOCK",		APPLY_BLOCK		},
    { "RESIST_SWORD",	APPLY_RESIST_SWORD	},
    { "RESIST_TWOHAND",	APPLY_RESIST_TWOHAND	},
    { "RESIST_DAGGER",	APPLY_RESIST_DAGGER    },
    { "RESIST_BELL",	APPLY_RESIST_BELL	},
    { "RESIST_FAN",	APPLY_RESIST_FAN	},
    { "RESIST_BOW",	APPLY_RESIST_BOW	},
    { "RESIST_FIRE",	APPLY_RESIST_FIRE	},
    { "RESIST_ELEC",	APPLY_RESIST_ELEC	},
    { "RESIST_MAGIC",	APPLY_RESIST_MAGIC	},
    { "RESIST_WIND",	APPLY_RESIST_WIND	},
	{ "REFLECT_MELEE",	APPLY_REFLECT_MELEE },
	{ "REFLECT_CURSE",	APPLY_REFLECT_CURSE },
    { "RESIST_ICE",		APPLY_RESIST_ICE	},
    { "RESIST_EARTH",	APPLY_RESIST_EARTH	},
    { "RESIST_DARK",	APPLY_RESIST_DARK	},
    { "RESIST_CRITICAL",	APPLY_ANTI_CRITICAL_PCT	},
    { "RESIST_PENETRATE",	APPLY_ANTI_PENETRATE_PCT	},
    { "POISON",		APPLY_POISON_PCT	},
    { "SLOW",		APPLY_SLOW_PCT		},
    { "STUN",		APPLY_STUN_PCT		},
    { "STEAL_HP",	APPLY_STEAL_HP		},
    { "STEAL_SP",	APPLY_STEAL_SP		},
    { "MANA_BURN_PCT",	APPLY_MANA_BURN_PCT	},
    { "CRITICAL",	APPLY_CRITICAL_PCT	},
    { "PENETRATE",	APPLY_PENETRATE_PCT	},
    { "KILL_SP_RECOVER",APPLY_KILL_SP_RECOVER	},
    { "KILL_HP_RECOVER",APPLY_KILL_HP_RECOVER	},
    { "PENETRATE_PCT",	APPLY_PENETRATE_PCT	},
    { "CRITICAL_PCT",	APPLY_CRITICAL_PCT	},
    { "POISON_PCT",	APPLY_POISON_PCT	},
    { "STUN_PCT",	APPLY_STUN_PCT		},
    { "ATT_BONUS_TO_WARRIOR",	APPLY_ATTBONUS_WARRIOR  },
    { "ATT_BONUS_TO_ASSASSIN",	APPLY_ATTBONUS_ASSASSIN },
    { "ATT_BONUS_TO_SURA",	APPLY_ATTBONUS_SURA	    },
    { "ATT_BONUS_TO_SHAMAN",	APPLY_ATTBONUS_SHAMAN   },
    { "ATT_BONUS_TO_MONSTER",	APPLY_ATTBONUS_MONSTER  },
    { "ATT_BONUS_TO_MOB",	APPLY_ATTBONUS_MONSTER  },
    { "MALL_ATTBONUS",	APPLY_MALL_ATTBONUS	},
    { "MALL_EXPBONUS",	APPLY_MALL_EXPBONUS	},
    { "MALL_DEFBONUS",	APPLY_MALL_DEFBONUS	},  
    { "MALL_ITEMBONUS",	APPLY_MALL_ITEMBONUS	},
    { "MALL_GOLDBONUS", APPLY_MALL_GOLDBONUS	},
    { "MAX_HP_PCT",	APPLY_MAX_HP_PCT	},
    { "MAX_SP_PCT",	APPLY_MAX_SP_PCT	},
    { "SKILL_DAMAGE_BONUS",	APPLY_SKILL_DAMAGE_BONUS	},
    { "NORMAL_HIT_DAMAGE_BONUS",APPLY_NORMAL_HIT_DAMAGE_BONUS	},
    { "SKILL_DEFEND_BONUS",	APPLY_SKILL_DEFEND_BONUS	},
    { "NORMAL_HIT_DEFEND_BONUS",APPLY_NORMAL_HIT_DEFEND_BONUS	},
    { "PCBANG_EXP_BONUS", APPLY_PC_BANG_EXP_BONUS	},
    { "PCBANG_DROP_BONUS", APPLY_PC_BANG_DROP_BONUS	},

    { "RESIST_WARRIOR",	APPLY_RESIST_WARRIOR},
    { "RESIST_ASSASSIN",	APPLY_RESIST_ASSASSIN},
    { "RESIST_SURA",		APPLY_RESIST_SURA},
    { "RESIST_SHAMAN",	APPLY_RESIST_SHAMAN},
    // by mhh game/affect.h 정의되어있음. INFINITE_AFFECT_DURATION = 0x1FFFFFFF
    { "INFINITE_AFFECT_DURATION", 0x1FFFFFFF	},
	{ "ENERGY", APPLY_ENERGY },		//	기력
	{ "COSTUME_ATTR_BONUS", APPLY_COSTUME_ATTR_BONUS },		//	기력
	{ "MAGIC_ATTBONUS_PER",	APPLY_MAGIC_ATTBONUS_PER	},
    { "MELEE_MAGIC_ATTBONUS_PER",	APPLY_MELEE_MAGIC_ATTBONUS_PER	},
#ifdef ENABLE_NEW_ATTR
	{ "ATTBONUS_METIN", APPLY_ATTBONUS_METIN },
#endif
#ifdef ENABLE_SHADOW_BONUS
	{ "ATTBONUS_SHADOW", APPLY_ATTBONUS_SHADOW },
#endif
#ifdef __NEW_ATTRIBUTES__
    { "ATTBONUS_METIN",				APPLY_ATTBONUS_METIN			},
    { "ATTBONUS_BOSS",				APPLY_ATTBONUS_BOSS				},	
#endif	
    { NULL,		0			}
};
// from import_item_proto.c

#ifdef ENABLE_NEW_PET_SYSTEM
DWORD * exppet_table = NULL;
//const DWORD pets = 4;
const DWORD Pet_Table[31][2] =
{
	{ 55701, 34041 }, // Monkey
	{ 55702, 34043 }, // Spider
	{ 55703, 34045 }, // Razador
	{ 55704, 34047 }, // Nemere
	{ 55705, 34049 }, // Blue Dragon
	{ 55706, 34051 }, // Red Dragon
	{ 55707, 34053 }, // Normal Azrael
	{ 55708, 34055 }, // Moppelchen
	{ 55709, 34057 }, // Baashido
	{ 55710, 34059 }, // Nessi
};

// Become more Bonus after incubation
const DWORD Special_Born_Attr_Table[SPECIAL_BORN_PETS_MAX_NUM][4][2] =
{
// 	ItemVnum || Min-Max -> HP / ATK / DEF
	{{55710}, { 25 , 40 }, { 25 , 40 }, { 25 , 40 }},		// Nessi
};

const DWORD Pet_Type_Attr_Table[9][3][2] =
{
// Min-Max -> HP / ATK / MOB
	{{ 1 , 23 }, { 1 , 15 }, { 1 , 15 }},	// Born Attr Values
	{{ 1 , 5 }, { 1 , 2 }, { 1 , 2 }},		// Typ 1
	{{ 2 , 3 }, { 2 , 3 }, { 2 , 3 }},		// Typ 2
	{{ 3 , 7 }, { 1 , 3 }, { 1 , 3 }},		// Typ 3
	{{ 3 , 4 }, { 3 , 4 }, { 3 , 4 }},		// Typ 4
	{{ 4 , 9 }, { 2 , 4 }, { 2 , 4 }},		// Typ 5
	{{ 5 , 6 }, { 4 , 5 }, { 4 , 5 }},		// Typ 6
	{{ 4 , 10}, { 3 , 7 }, { 3 , 7 }},		// Typ 7
	{{ 6 , 8 }, { 5 , 7 }, { 5 , 7 }}		// Typ 8
};

const DWORD Pet_Skill_Table[23][23] =
{
	//Affect Value | Active/Passive | Cooltime | Values for Level ->
	{ 78,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 },							// 1 Resist Warrior
	{ 80,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 },							// 2 Resist Sura
	{ 79,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 },							// 3 Resist Assassin 
	{ 81,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 },							// 4 Resist Shaman
	{ 93,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 11, 12 },							// 5 Resist Lycan
	{ 53,		0,		0,			1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10 },							// 6 Beserker
	{ 97,		0,		0,			1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 7, 7, 8, 8, 9, 10, 10, 11, 11, 12 },							// 7 Anti Magic
	{ 9,		0,		0,			8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 17, 17 },				// 8 Haste (Skill Cooldown reduce)
	{ 16,		0,		0,			1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9 },								// 9 Drill
	{ 4,		1,		300,		500, 750, 900, 1280, 1560, 1800, 2010, 2370, 2720, 3050, 3590, 4270, 4780, 5950, 7200, 8540, 10320, 12400, 15600, 20000 }, // 10 Restoration
	{ 23,		0,		0,			1, 1, 1, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5 },											// 11 Vampirism
	{ 24,		0,		0,			1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 6, 6, 6 },											// 12 Spiritualism
	{ 27,		0,		0,			1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10, 11, 12 },										// 13 Bulwark
	{ 28,		0,		0,			1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 7, 8 },											// 14 Reflection
	{ 44,		0,		0,			5, 10, 15, 20, 25, 31, 36, 41, 46, 51, 56, 61, 67, 72, 77, 82, 87, 92, 97, 103 }, 						// 15 Yang Drop
	{ 52,		0,		0,			1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6 },											// 16 Bow-Range Bonus
	{ 11,		1,		600,		10, 12, 15, 19, 22, 26, 31, 38, 45, 54, 60, 66, 73, 80, 88, 97, 108, 120, 133, 150 },					// 17 Immortal
	{ 6,		1,		150,		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },											// 18 Panacea
	{ 46,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20 },									// 19 Master Brew
	{ 63,		0,		0,			1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 12, 15 },										// 20 Monster Hunter
	{ 66,		0,		0,			1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 16, 18, 20 },									// 21 Eagle Eyes
	{ 47,		0,		0,			1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 7, 8, 9, 10 },											// 22 Life Drain
	{ 6,		1,		150,		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },											// 23 Featherlight
};

const DWORD Pet_SKill_TablePerc[5][21] =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },							// None
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 24, 27, 30, 33, 35, 38, 41, 44, 46, 49, 52, 55 },			// 10 Restoration
	{ 0, 2, 5, 7, 10, 12, 15, 17, 20, 23, 25, 27, 30, 32, 35, 37, 40, 42, 45, 47, 50 },			// 17 Immortal
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 25, 27, 30, 33, 36, 38, 41, 44, 47, 50, 52, 55 },			// 18 Panacea
	{ 0, 2, 5, 8, 11, 13, 16, 19, 22, 25, 27, 30, 33, 36, 38, 41, 44, 47, 50, 52, 55 },			// 23 Featherlight
};

DWORD exppet_table_common[121];
#endif

long FN_get_apply_type(const char *apply_type_string)
{
	TValueName	*value_name;
	for (value_name = c_aApplyTypeNames; value_name->c_pszName; ++value_name)
	{
		if (0==strcasecmp(value_name->c_pszName, apply_type_string))
			return value_name->lValue;
	}
	return 0;
}

// new stuff
const uint8_t arrSkills[JOB_MAX_NUM][SKILL_GROUP_MAX_NUM][6] =
{
	{
		{SKILL_GEOMKYUNG, SKILL_JEONGWI, SKILL_SAMYEON, SKILL_PALBANG, SKILL_TANHWAN, SKILL_RESERVED},
		{SKILL_CHUNKEON, SKILL_GIGONGCHAM, SKILL_GYOKSAN, SKILL_DAEJINGAK, SKILL_GEOMPUNG, SKILL_RESERVED},
	},
	{
		{SKILL_EUNHYUNG, SKILL_SANGONG, SKILL_CHARYUN, SKILL_GUNGSIN, SKILL_AMSEOP, SKILL_RESERVED},
		{SKILL_GYEONGGONG, SKILL_GIGUNG, SKILL_HWAJO, SKILL_KWANKYEOK, SKILL_YEONSA, SKILL_RESERVED},
	},
	{
		{SKILL_GWIGEOM, SKILL_TERROR, SKILL_JUMAGAP, SKILL_PABEOB, SKILL_SWAERYUNG, SKILL_YONGKWON},
		{SKILL_MUYEONG, SKILL_MANASHILED, SKILL_TUSOK, SKILL_MAHWAN, SKILL_HWAYEOMPOK, SKILL_MARYUNG},
	},
	{
		{SKILL_HOSIN, SKILL_REFLECT, SKILL_GICHEON, SKILL_BIPABU, SKILL_YONGBI, SKILL_PAERYONG},
		{SKILL_JEONGEOP, SKILL_JEUNGRYEOK, SKILL_KWAESOK, SKILL_NOEJEON, SKILL_BYEURAK, SKILL_CHAIN},
	},
};




#ifdef ENABLE_BIYOLOG
const BYTE bio_max = 15;
BYTE pointToApply(BYTE p)
{
	switch (p)
	{
		case POINT_MOV_SPEED:
			return APPLY_MOV_SPEED;
		case POINT_ATT_SPEED:
			return APPLY_ATT_SPEED;
		case POINT_DEF_GRADE_BONUS:
			return APPLY_DEF_GRADE_BONUS;
		case POINT_ATT_GRADE_BONUS:
			return APPLY_ATT_GRADE_BONUS;
		case POINT_RESIST_WARRIOR:
			return APPLY_RESIST_WARRIOR;
		case POINT_RESIST_ASSASSIN:
			return APPLY_RESIST_ASSASSIN;
		case POINT_RESIST_SURA:
			return APPLY_RESIST_SURA;
		case POINT_RESIST_SHAMAN:
			return APPLY_RESIST_SHAMAN;
		case POINT_ATTBONUS_WARRIOR:
			return APPLY_ATTBONUS_WARRIOR;
		case POINT_ATTBONUS_ASSASSIN:
			return APPLY_ATTBONUS_ASSASSIN;
		case POINT_ATTBONUS_SURA:
			return APPLY_ATTBONUS_SURA;
		case POINT_ATTBONUS_SHAMAN:
			return APPLY_ATTBONUS_SHAMAN;
		case POINT_MAX_HP:
			return APPLY_MAX_HP;
	}
	return APPLY_NONE;
}

const DWORD bio_data[][17] = {
	// level, item, count, time, lucky, stone, afftype1, affvalue1, afftype2, affvalue2, afftype3, affvalue3 , afftype4, affvalue4, isChoose
		{ 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0},//empty
		{ 20, 53560, 5, 600, 60, 53574, POINT_ATTBONUS_ORC, 25,0,0,0,0,0,0,0, 71035, 70022},//Leg. Orkzahn (Lev. 20)
		{ 50, 53561, 8, 900, 60, 53574, POINT_ATTBONUS_ANIMAL, 25,POINT_ATTBONUS_MILGYO,25,0,0,0,0,0, 71035, 70022},//Leg. Arachnidenflote (Lev. 50)
		{ 55, 53562, 10, 900, 60, 53574, POINT_ATTBONUS_UNDEAD, 20,POINT_ATTBONUS_DEVIL,20,0,0,0,0,0, 71035, 70022},//Leg. Sense (Lev. 55)
		{ 60, 53563, 15, 900, 60, 53574, POINT_ATTBONUS_MONSTER, 10,0,0,0,0,0,0,0, 71035, 70022},//Leg. Horn des Azrael (Lev. 60)
		{ 75, 53564, 20, 1200, 60, 53574, POINT_ATTBONUS_BOSS, 10,POINT_EXP_DOUBLE_BONUS,20,0,0,0,0,0, 71035, 70022},//Leg. Drachenauge (Lev. 75)
		{ 80, 53565, 20, 1200, 60, 53574, POINT_ATTBONUS_METIN, 10,0,0,0,0,0,0,0, 71035, 70022},//Leg. Horn Razador (Lev. 80)
		{ 85, 53566, 25, 1200, 60, 53574, POINT_ATTBONUS_MONSTER, 15,0,0,0,0,0,0,0, 71035, 70022},//Leg. Horn des Nemere (Lev. 85)
		{ 90, 53567, 25, 1200, 60, 53574, POINT_ATTBONUS_HUMAN, 10,POINT_MAX_HP,3000,0,0,0,0,0, 71035, 70022},//Leg. Klinge des Jotun Thyrm (Lev. 90)
		{ 105, 53568, 25, 1800, 60, 53574, POINT_SKILL_DEFEND_BONUS, 10,0,0,0,0,0,0,0, 71035, 70022},//Leg. Sphinx Kralle (Lev. 105)
		{ 110, 53569, 30, 2700, 60, 53574, POINT_NORMAL_HIT_DEFEND_BONUS, 10,0,0,0,0,0,0,0, 71035, 70022},//Leg. Schwert des Kaisers (Lev. 110)
		{ 115, 53570, 30, 2700, 60, 53574, POINT_ATTBONUS_DEVIL, 20,0,0,0,0,0,0,0, 71035, 70022},//Leg. Verlorene Seele (Lev. 115)
		{ 118, 53571, 30, 3600, 60, 53574, POINT_ATTBONUS_METIN, 10,0,0,0,0,0,0,0, 71035, 70022},//Leg. Sense von Fandalia (Lev. 118)
		{ 120, 53572, 30, 3600, 60, 53574, POINT_NORMAL_HIT_DAMAGE_BONUS, 5,POINT_SKILL_DAMAGE_BONUS,5,0,0,0,0,0, 71035, 70022},//Leg. Stab von Antlantis (Lev. 120)
};
#endif
