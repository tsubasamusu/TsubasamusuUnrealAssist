#pragma once

#include "CoreMinimal.h"
#include "CommentNodeType.generated.h"

UENUM()
enum class ECommentNodeType
{
	Normal,
	Gaming,
	Pong UMETA(DisplayName = "PONG")
};
