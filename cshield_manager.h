#pragma once

class CShieldManager : public singleton<CShieldManager>
{
public:
	bool		Initialize();
	void		Destroy();
	void		CShieldUpdate();

};