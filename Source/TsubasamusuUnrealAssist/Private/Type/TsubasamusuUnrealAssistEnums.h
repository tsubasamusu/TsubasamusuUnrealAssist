// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

enum class ETsubasamusuDialogButton : uint8
{
	OK,
	Cancel,
	Close
};

UENUM()
enum class ETsubasamusuAccessSpecifier : uint8
{
	None,
	Private,
	Protected,
	Public
};

enum class ELlamaServerStatus : uint8
{
	NotStartedYet,
	SuccessfullyStarted,
	FailedToStart
};
