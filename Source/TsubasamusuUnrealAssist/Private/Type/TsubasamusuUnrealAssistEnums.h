// Copyright (c) 2026, tsubasamusu All rights reserved.

#pragma once

namespace TsubasamusuUnrealAssist
{
	enum class EDialogButton : uint8
	{
		OK,
		Cancel,
		Close
	};
	
	enum class EBlueprintMember : uint8
	{
		None,
		Variable,
		Function,
		CustomEvent
	};
	
	enum class EAccessSpecifier : uint8
	{
		None,
		Private,
		Protected,
		Public
	};
}
