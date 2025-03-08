class CHARACTER;

enum 
{
	EnumStep1ItemVnum = 53090,
	EnumStep2ItemVnum = 53091,
	EnumStep3ItemVnum = 53092,
	
};


class CDitoActor
{
	private:
		LPCHARACTER m_pkActor;// BUffer
		LPCHARACTER m_pkOwner;// Owner
		LPEVENT	m_pkDitoSystemUpdateEvent;
		DWORD m_dwLevel;
	public:
		CDitoActor();
		~CDitoActor();
		bool IsSummoned() { return m_pkActor != NULL; }
		bool SetOwner(LPCHARACTER pkOwner);
		bool Summon(); 
		void Unsummon();
		bool Update();
		bool Follow(float fMinDistance);
		bool _UpdateFollowAI();
		DWORD GetLevel() { return m_dwLevel; }
		void SetLevel(DWORD dwLevel) { m_dwLevel=dwLevel; }
		int GetValue(int max_val) { return (int)(max_val*((float)GetLevel()/100)); }
		
		LPCHARACTER GetOwner() { return m_pkOwner; }
		LPCHARACTER GetActor() { return m_pkActor; }
};


