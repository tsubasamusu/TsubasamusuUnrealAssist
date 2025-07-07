// Copyright (c) 2025, tsubasamusu All rights reserved.

#include "Setting/TsubasamusuSettingsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widget/SLanguageComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "IDocumentation.h"

#define LOCTEXT_NAMESPACE "TsubasamusuSettingsCustomization"

TSharedRef<IDetailCustomization> FTsubasamusuSettingsCustomization::Create()
{
    return MakeShared<FTsubasamusuSettingsCustomization>();
}

void FTsubasamusuSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	ChangeOpenAiApiKeyPropertyDisplayAsPassword(DetailBuilder);
	AddGptLanguageProperty(DetailBuilder);
    AddGptModelsDocumentButton(DetailBuilder);
}

void FTsubasamusuSettingsCustomization::ChangeOpenAiApiKeyPropertyDisplayAsPassword(IDetailLayoutBuilder& DetailLayout)
{
    const FName CategoryName = TEXT("Comment Generation");
    const FName PropertyName = TEXT("OpenAiApiKey");

    IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory(CategoryName);
    check(!DetailCategoryBuilder.IsEmpty());

    TArray<TSharedRef<IPropertyHandle>> PropertyHandles;
    DetailCategoryBuilder.GetDefaultProperties(PropertyHandles);

    for (const TSharedRef<IPropertyHandle>& PropertyHandle : PropertyHandles)
    {
        if (PropertyHandle->GetProperty()->GetName() != PropertyName)
        {
            continue;
        }

        DetailCategoryBuilder.AddProperty(PropertyHandle)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(250)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SEditableTextBox)
                .Text_Lambda([PropertyHandle]()
                {
                    FString Value;
                    PropertyHandle->GetValue(Value);
                    return FText::FromString(Value);
                })
                .OnTextCommitted_Lambda([PropertyHandle](const FText& NewText, ETextCommit::Type)
                {
                    PropertyHandle->SetValue(NewText.ToString());
                })
                .IsPassword(true)
            ]
        ];
    }
}

void FTsubasamusuSettingsCustomization::AddGptLanguageProperty(IDetailLayoutBuilder& DetailLayout)
{
    const FName CategoryName = TEXT("Comment Generation");
    const FText CategoryText = LOCTEXT("CategoryText", "Comment Generation");
    const FText PropertyText = LOCTEXT("PropertyText", "GPT Language");

    IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory(CategoryName, CategoryText);
    check(!DetailCategoryBuilder.IsEmpty());

    const TSharedRef<FLocalizedCulturesFlyweight> LocalizedCulturesFlyweight = MakeShared<FLocalizedCulturesFlyweight>();

    DetailCategoryBuilder.AddCustomRow(PropertyText)
    .NameContent()
    [
        SNew(STextBlock)
        .Text(PropertyText)
        .Font(DetailLayout.GetDetailFont())
    ]
    .ValueContent()
    [
        SNew(SLanguageComboButton, LocalizedCulturesFlyweight)
    ];
}

void FTsubasamusuSettingsCustomization::AddGptModelsDocumentButton(IDetailLayoutBuilder& DetailLayout)
{
    const FName CategoryName = TEXT("Comment Generation");
    const FName PropertyName = TEXT("GptModelName");

    IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory(CategoryName);
    check(!DetailCategoryBuilder.IsEmpty());

    TArray<TSharedRef<IPropertyHandle>> PropertyHandles;
    DetailCategoryBuilder.GetDefaultProperties(PropertyHandles);

    for (const TSharedRef<IPropertyHandle>& PropertyHandle : PropertyHandles)
    {
        if (PropertyHandle->GetProperty()->GetName() != PropertyName)
        {
            continue;
        }

        DetailCategoryBuilder.AddProperty(PropertyHandle)
        .CustomWidget()
        .NameContent()
        [
            PropertyHandle->CreatePropertyNameWidget()
        ]
        .ValueContent()
        .MinDesiredWidth(200.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SEditableTextBox)
                .Font(FAppStyle::GetFontStyle("PropertyWindow.NormalFont"))
                .HintText(FText::FromString(TEXT("gpt-4")))
                .Text_Lambda([PropertyHandle]()
                {
                    FString Value;
                    PropertyHandle->GetValue(Value);
                    return FText::FromString(Value);
                })
                .OnTextCommitted_Lambda([PropertyHandle](const FText& NewText, ETextCommit::Type)
                {
                    PropertyHandle->SetValue(NewText.ToString());
                })
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(5.f, 0.f, 0.f, 0.f)
            [
                SNew(SButton)
                .ButtonStyle(FAppStyle::Get(), TEXT("NoBorder"))
                .ButtonColorAndOpacity(FLinearColor::White)
                .ToolTipText(FText::FromString(TEXT("View the types of GPT models currently available")))
                .OnClicked_Lambda([]()
                {
                    // ReSharper disable once CppExpressionWithoutSideEffects
                    IDocumentation::Get()->Open(TEXT("https://platform.openai.com/docs/models"));
                    return FReply::Handled();
                })
                [
                    SNew(SBox)
                    .HAlign(HAlign_Fill)
                    .VAlign(VAlign_Fill)
                    [
                        SNew(SImage)
                        .ColorAndOpacity(FLinearColor::White)
                        .Image(FAppStyle::Get().GetBrush(TEXT("MainFrame.VisitSearchForAnswersPage")))
                    ]
                ]
            ]
        ];
    }
}

#undef LOCTEXT_NAMESPACE
