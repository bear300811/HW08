#include "BigCoinItem.h"

ABigCoinItem::ABigCoinItem() 
{
	PointValue = 50;
	Itemtype = "BigCoin";

}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}