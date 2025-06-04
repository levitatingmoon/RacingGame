// Copyright 2025 Teyon. All Rights Reserved.


#include "RaceWidget.h"
#include "PraktykiGameModeBase.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
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

			SectorImages.Add(SectorImage);
		}
	}

}

void URaceWidget::UpdateSectorBox()
{

	GetWorld()->GetTimerManager().SetTimer(SectorColourTimer, this, &URaceWidget::ResetSectorBoxes, 2.0f, false);

}

void URaceWidget::ShowPenalty()
{
	PenaltyText->SetVisibility(ESlateVisibility::Visible);
	PenaltyBackground->SetVisibility(ESlateVisibility::Visible);

}

void URaceWidget::HidePenalty()
{
	PenaltyText->SetVisibility(ESlateVisibility::Collapsed);
	PenaltyBackground->SetVisibility(ESlateVisibility::Collapsed);
}

void URaceWidget::ResetSectorBoxes()
{
	for (UImage* Image : SectorImages)
	{
		Image->SetColorAndOpacity(FLinearColor::Black);
	}
}

void URaceWidget::ShowSectorDifference()
{
	//SectorTimeDifference->SetVisibility(ESlateVisibility::Visible);
}

void URaceWidget::HideSectorDifference()
{
	//SectorTimeDifference->SetVisibility(ESlateVisibility::Collapsed);
}
