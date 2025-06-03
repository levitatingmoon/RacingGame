// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceWidget.h"
#include "PraktykiGameModeBase.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "RacingCar.h"


void URaceWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void URaceWidget::CreateSectorBox()
{
	SectorBox->ClearChildren();

	APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		for (int i = 0; i < GameMode->SectorNumber; i++)
		{
			UImage* SectorImage = NewObject<UImage>(this);

			//SectorImage->SetColorAndOpacity(Color);
			SectorImage->SetDesiredSizeOverride(FVector2D(30.f, 30.f));
			SectorImage->SetColorAndOpacity(FLinearColor(FColor(24, 24, 25, 255)));

			UHorizontalBoxSlot* BoxSlot = SectorBox->AddChildToHorizontalBox(SectorImage);

			if (BoxSlot)
			{
				BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
				BoxSlot->SetPadding(FMargin(2.0f));
				BoxSlot->SetHorizontalAlignment(HAlign_Fill);
				BoxSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}

}

void URaceWidget::UpdateSectorBox(int Index)
{
	APraktykiGameModeBase* GameMode = Cast<APraktykiGameModeBase>(GetWorld()->GetAuthGameMode());
	if (SectorBox->HasAnyChildren())
	{
		UWidget* Child = SectorBox->GetChildAt(Index);
		if (Child)
		{
			UImage* SectorImage = Cast<UImage>(Child);
			if (SectorImage)
			{
				if (Index != 0)
				{
					if (OwningRacingCar->CurrentSectorTimes[Index - 1] < OwningRacingCar->BestSectorTimes[Index - 1])
					{
						SectorImage->SetColorAndOpacity(FLinearColor::Green);
					}
					else
					{
						SectorImage->SetColorAndOpacity(FLinearColor::Yellow);
					}
				}
				else
				{
					
					if (OwningRacingCar->CurrentSectorTimes[GameMode->SectorNumber - 1] < OwningRacingCar->BestSectorTimes[GameMode->SectorNumber - 1])
					{
						SectorImage->SetColorAndOpacity(FLinearColor::Green);
					}
					else
					{
						SectorImage->SetColorAndOpacity(FLinearColor::Yellow);
					}
				}

			}
		}
	}
}
