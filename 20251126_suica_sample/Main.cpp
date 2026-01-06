# include <Siv3D.hpp> // Siv3D v0.6.15
# include "Fruits.h"
# include "GameManager.h"
# include "Vector3.h"

void Main()
{
	GameManager gm = {};

	gm.Initialize();

	Vector3 vector(1,1,1);

	while (System::Update())
	{
		gm.Update();
	}
}
