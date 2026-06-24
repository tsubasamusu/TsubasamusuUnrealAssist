# Tsubasamusu Unreal Assist (Experimental)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tsubasamusu/TsubasamusuUnrealAssist)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![UE Version](https://img.shields.io/badge/Unreal%20Engine-5.0--5.8-0e1128?logo=unrealengine&logoColor=white)
[![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/tsubasamusu?style=social)](https://twitter.com/tsubasamusu)

> [!WARNING]
> The content of this file is an automatic translation of [README_ja.md](https://github.com/tsubasamusu/TsubasamusuUnrealAssist/blob/main/README_ja.md) generated using AI.

## Overview
An Unreal Editor extension plugin that adds features I personally want in UE5, as well as features I've seen on social media that people want. When this plugin is enabled, a "Tsubasamusu Unreal Assist" category is added to "Editor Preferences > Plugins", where you can change settings related to this plugin.

> [!CAUTION]
> This plugin is an experimental feature and is released under the [MIT License](https://opensource.org/license/mit). We are not responsible for any issues that arise from using this plugin.

> [!WARNING]
> - Currently, only the Win64 platform is supported.
> - Most of the features provided by this plugin are related to Blueprint, but since Blueprint themself may be deprecated in UE6 and later, I plan to continue development using a separate plugin called “[Tsubasamusu Unreal Assist for UE6](https://github.com/tsubasamusu/tua4ue6)” for UE6 and later versions.

## Engine Version
Supported engine versions are as follows:

- 5.0
- 5.1
- 5.2
- 5.3
- 5.4
- 5.5
- 5.6
- 5.7
- 5.8

## Installation Instructions
### Installing the Plugin
1. Download the "Source code (zip)" from the [latest release](https://github.com/tsubasamusu/TsubasamusuUnrealAssist/releases/latest).
2. Extract the downloaded ZIP file to the Plugins folder of the engine or UE project.
3. Open the UE project and enable "Tsubasamusu Unreal Assist" in the "Editor" category under "Edit > Plugins" and restart the editor.
4. Build from IDE if necessary.
5. If "Tsubasamusu Unreal Assist" category has been added to "Editor Preferences > Plugins", you're all set.

### Using Local LLM
Currently, this plugin uses a local LLM in the Comment Generator feature. For local LLM usage, we use the "LLaMA.cpp HTTP Server" provided by [llama.cpp](https://github.com/ggml-org/llama.cpp), a lightweight execution engine for running LLMs in a local environment.

The setup instructions for this plugin are as follows:

> [!WARNING]
> Since the supported platform for this plugin is Win64 only, the following instructions are for a Win64 environment.

1. Download "llama-{build-number}-bin-win-{type}-x64.zip" from the [latest llama.cpp release](https://github.com/ggml-org/llama.cpp/releases/latest). The recommended versions are "llama-b8475-bin-win-**cpu**-x64.zip" and "llama-b8475-bin-win-**vulkan**-x64.zip", but please download the one that matches your environment.
2. Extract the downloaded ZIP file.
3. Open the Unreal Editor and set the path to "llama-server.exe" in the extracted files to "Editor Preferences > Plugins > Tsubasamusu Unreal Assist > LLM > Llama Server File Path".
4. Download a GGUF file of the model you want to use from [Hugging Face](https://huggingface.co).
5. Add the "Model" option to "Editor Preferences > Plugins > Tsubasamusu Unreal Assist > LLM > Llama Server Options" and set the path of the downloaded GGUF file to "LLM File Path". You can also set several other options here, but you can use the local LLM with just the "Model" option configured at minimum.
6. A button labeled "Start Llama Server" will be displayed along with a warning message. Click it. If "Llama Server File Path" and "LLM File Path" are configured correctly, the server will start successfully and the warning message will no longer be displayed. Now you're ready to use the local LLM.

<img width="700" src="https://github.com/user-attachments/assets/8d1940d9-c609-4ebb-bbe3-4949ee3d4010">

## Features
### Comment Generator
When you right-click on a comment node containing one or more other nodes inside it, a "Generate Comment" menu appears. By clicking this menu, you can generate comments using a local LLM.

<img width="700" src="https://github.com/user-attachments/assets/fd577e3e-6ea9-4047-811e-67e29e412071">

The editor settings related to this feature are as follows:

| Property Name | Overview |
----|----
| Use Editor Language | Whether to set the language of generated comments to the same as the editor language. |
| Language | The language for generating comments. This property can be edited when "Use Editor Language" is unchecked. Conversely, when "Use Editor Language" is checked, this property's value becomes the same as the editor language. ※Depending on the LLM used, it may not generate comments in the specified language. |
| Enable Streaming Generation | Whether to enable streaming generation of comments. When this property is checked, comments are generated and reflected token-by-token like chat screens in ChatGPT, Claude, Gemini, etc. When unchecked, comments are updated once after generation is complete. |
| Ignore Nodes Do Not Have Connected Pins | Whether to ignore nodes that have no input pins, output pins, execution pins, etc. connected to other pins when generating comments. |
| Ignore Comment Nodes | Whether to ignore other comment nodes within the comment node when generating comments. |
| Use TOON Format | When passing node information within a comment node to the LLM, whether to pass it as a [TOON](https://toonformat.dev) format string. When this property is unchecked, JSON is used instead of TOON. TOON can reduce the number of tokens compared to JSON. |
| Conditions | Conditions that the LLM follows when generating comments. For example, "summarize what is being done in the blueprint concisely" or "generate in a da-aru tone". |

### Comment Translator
When you right-click on a comment node with a comment set, a "Translate to..." menu appears. When you hover your mouse over this menu, a list of target languages appears. Selecting any of these languages allows you to translate the comment written in the comment node to that language.

> [!NOTE]
> To use this feature, you must pre-configure the API key in the editor settings' "Comment Translator > DeepL API Key".

<img width="500" src="https://github.com/user-attachments/assets/1f25d9e4-5037-42c7-b561-46e56d176f39">

### Array Node Creator
When you select multiple nodes with output pins of the same type and right-click, a "Make Array" menu appears. Selecting this menu allows you to instantly create a Make Array node connected to those output pins and add it to the graph.

<img width="500" src="https://github.com/user-attachments/assets/1b7cf903-4eba-44a6-b475-5e513eb4b4d5">

### Node Information Copier
When you have one or more nodes selected and right-click, a "Copy as..." menu appears. When you hover your mouse over this menu, two items appear: "JSON" and "TOON". Selecting either allows you to copy the information of those nodes to the clipboard as a JSON or [TOON](https://toonformat.dev) format string. This string includes the node name, pin name, node comment, pin direction (input or output), pin type name, pin GUID, and the GUIDs of pins connected to that pin, etc. When you select "JSON", you can copy approximately one-third of the number of characters that would be copied with Unreal Engine's standard copy function. Furthermore, when you select "TOON", you can copy about half the number of characters compared to JSON.

<img width="700" src="https://github.com/user-attachments/assets/7a4b25c9-46c1-4b39-a6b6-09251ca1801b">

For example, the blueprint shown in the image above is a level blueprint of a level called "L_Sample", and it displays either "Nice to meet you!!" or "We meet again!" on the screen depending on the value of a bool type variable called "IsFirstPlay". The actual string copied using this plugin's copy function is as follows:

<details>

<summary>JSON</summary>

```json
{
	"nodeDataList": [
		{
			"nodeName": "Get IsFirstPlay",
			"comment": "",
			"bIsCommentNode": false,
			"pinDataList": [
				{
					"pinName": "Is First Play",
					"pinDirection": "Output",
					"pinType": "Boolean",
					"pinId": "38F0BD7B4548CE3707F161951E77EFD2",
					"defaultValue": "false",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"41AF610842826F7EA4CF1C939BC0BD14"
					]
				},
				{
					"pinName": "Target",
					"pinDirection": "Input",
					"pinType": "L Sample",
					"pinId": "683AF56545EA5021298A688DCB203779",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				}
			]
		},
		{
			"nodeName": "Event BeginPlay",
			"comment": "",
			"bIsCommentNode": false,
			"pinDataList": [
				{
					"pinName": "Output Delegate",
					"pinDirection": "Output",
					"pinType": "Delegate",
					"pinId": "F929107B41909F2A9DE7C58E3F17DF66",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": []
				},
				{
					"pinName": "then",
					"pinDirection": "Output",
					"pinType": "Exec",
					"pinId": "A1F7AC82495D21755E9AEC907008A979",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"D886E14F4E4EB1489CD67D9AE743AAD7"
					]
				}
			]
		},
		{
			"nodeName": "Branch",
			"comment": "",
			"bIsCommentNode": false,
			"pinDataList": [
				{
					"pinName": "execute",
					"pinDirection": "Input",
					"pinType": "Exec",
					"pinId": "D886E14F4E4EB1489CD67D9AE743AAD7",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"A1F7AC82495D21755E9AEC907008A979"
					]
				},
				{
					"pinName": "Condition",
					"pinDirection": "Input",
					"pinType": "Boolean",
					"pinId": "41AF610842826F7EA4CF1C939BC0BD14",
					"defaultValue": "true",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"38F0BD7B4548CE3707F161951E77EFD2"
					]
				},
				{
					"pinName": "True",
					"pinDirection": "Output",
					"pinType": "Exec",
					"pinId": "F1AF8EF8433BA25A83CB75BC7393D7EB",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"38A2D2F64DC6046FF9D271A4A76C63C8"
					]
				},
				{
					"pinName": "False",
					"pinDirection": "Output",
					"pinType": "Exec",
					"pinId": "7EE5C76349D1514966AF38BBB160EAC3",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"38A2D2F64DC6046FF9D271A4A76C63C8"
					]
				}
			]
		},
		{
			"nodeName": "Print String",
			"comment": "",
			"bIsCommentNode": false,
			"pinDataList": [
				{
					"pinName": "execute",
					"pinDirection": "Input",
					"pinType": "Exec",
					"pinId": "38A2D2F64DC6046FF9D271A4A76C63C8",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"F1AF8EF8433BA25A83CB75BC7393D7EB"
					]
				},
				{
					"pinName": "then",
					"pinDirection": "Output",
					"pinType": "Exec",
					"pinId": "9C0778834E4A81758E83FA834E91B2AD",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": []
				},
				{
					"pinName": "Target",
					"pinDirection": "Input",
					"pinType": "Kismet System Library",
					"pinId": "FF22BD2F4D6C374F30E125903EEA1C70",
					"defaultValue": "/Script/Engine.Default__KismetSystemLibrary",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "World Context Object",
					"pinDirection": "Input",
					"pinType": "Object",
					"pinId": "6FAB1F2E40E482619454AA842B390F56",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "In String",
					"pinDirection": "Input",
					"pinType": "String",
					"pinId": "C21184844D66411D3D89EE8DFB5DD89B",
					"defaultValue": "Nice to meet you!!",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Print to Screen",
					"pinDirection": "Input",
					"pinType": "Boolean",
					"pinId": "EC90BC1046D7AB1FBEB309A15DF87FA4",
					"defaultValue": "true",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Print to Log",
					"pinDirection": "Input",
					"pinType": "Boolean",
					"pinId": "E1A51F0549680735E8DC549ECED33D09",
					"defaultValue": "true",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Text Color",
					"pinDirection": "Input",
					"pinType": "Linear Color",
					"pinId": "B6D72FF44EF181599820E28881FC973A",
					"defaultValue": "(R=0.000000,G=1.000000,B=0.000000,A=1.000000)",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Duration",
					"pinDirection": "Input",
					"pinType": "Float (single-precision)",
					"pinId": "A2CC654440E553AC90FA03ABB7F3E60B",
					"defaultValue": "10.000000",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Key",
					"pinDirection": "Input",
					"pinType": "Name",
					"pinId": "F7F12F7846161B2188E38EB89836CD81",
					"defaultValue": "None",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				}
			]
		},
		{
			"nodeName": "Print String",
			"comment": "",
			"bIsCommentNode": false,
			"pinDataList": [
				{
					"pinName": "execute",
					"pinDirection": "Input",
					"pinType": "Exec",
					"pinId": "38A2D2F64DC6046FF9D271A4A76C63C8",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": [
						"7EE5C76349D1514966AF38BBB160EAC3"
					]
				},
				{
					"pinName": "then",
					"pinDirection": "Output",
					"pinType": "Exec",
					"pinId": "9C0778834E4A81758E83FA834E91B2AD",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": false,
					"connectedPinIds": []
				},
				{
					"pinName": "Target",
					"pinDirection": "Input",
					"pinType": "Kismet System Library",
					"pinId": "FF22BD2F4D6C374F30E125903EEA1C70",
					"defaultValue": "/Script/Engine.Default__KismetSystemLibrary",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "World Context Object",
					"pinDirection": "Input",
					"pinType": "Object",
					"pinId": "6FAB1F2E40E482619454AA842B390F56",
					"defaultValue": "",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "In String",
					"pinDirection": "Input",
					"pinType": "String",
					"pinId": "C21184844D66411D3D89EE8DFB5DD89B",
					"defaultValue": "We meet again!",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Print to Screen",
					"pinDirection": "Input",
					"pinType": "Boolean",
					"pinId": "EC90BC1046D7AB1FBEB309A15DF87FA4",
					"defaultValue": "true",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Print to Log",
					"pinDirection": "Input",
					"pinType": "Boolean",
					"pinId": "E1A51F0549680735E8DC549ECED33D09",
					"defaultValue": "true",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Text Color",
					"pinDirection": "Input",
					"pinType": "Linear Color",
					"pinId": "B6D72FF44EF181599820E28881FC973A",
					"defaultValue": "(R=1.000000,G=0.000000,B=0.000000,A=1.000000)",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Duration",
					"pinDirection": "Input",
					"pinType": "Float (single-precision)",
					"pinId": "A2CC654440E553AC90FA03ABB7F3E60B",
					"defaultValue": "10.000000",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				},
				{
					"pinName": "Key",
					"pinDirection": "Input",
					"pinType": "Name",
					"pinId": "F7F12F7846161B2188E38EB89836CD81",
					"defaultValue": "None",
					"bThisPinUsesDefaultValue": true,
					"connectedPinIds": []
				}
			]
		}
	]
}
```

</details>

<details>

<summary>TOON</summary>

```
nodeDataList[5]{nodeName,comment,bIsCommentNode,pinDataList}:
  Get IsFirstPlay,,false,
    pinDataList[2]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:
      Is First Play,Output,Boolean,38F0BD7B4548CE3707F161951E77EFD2,false,false,[41AF610842826F7EA4CF1C939BC0BD14]
      Target,Input,L Sample,683AF56545EA5021298A688DCB203779,,true,[]
  Event BeginPlay,,false,
    pinDataList[2]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:
      Output Delegate,Output,Delegate,F929107B41909F2A9DE7C58E3F17DF66,,false,[]
      then,Output,Exec,A1F7AC82495D21755E9AEC907008A979,,false,[D886E14F4E4EB1489CD67D9AE743AAD7]
  Branch,,false,
    pinDataList[4]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:
      execute,Input,Exec,D886E14F4E4EB1489CD67D9AE743AAD7,,false,[A1F7AC82495D21755E9AEC907008A979]
      Condition,Input,Boolean,41AF610842826F7EA4CF1C939BC0BD14,true,false,[38F0BD7B4548CE3707F161951E77EFD2]
      True,Output,Exec,F1AF8EF8433BA25A83CB75BC7393D7EB,,false,[38A2D2F64DC6046FF9D271A4A76C63C8]
      False,Output,Exec,7EE5C76349D1514966AF38BBB160EAC3,,false,[38A2D2F64DC6046FF9D271A4A76C63C8]
  Print String,,false,
    pinDataList[10]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:
      execute,Input,Exec,38A2D2F64DC6046FF9D271A4A76C63C8,,false,[F1AF8EF8433BA25A83CB75BC7393D7EB]
      then,Output,Exec,9C0778834E4A81758E83FA834E91B2AD,,false,[]
      Target,Input,Kismet System Library,FF22BD2F4D6C374F30E125903EEA1C70,/Script/Engine.Default__KismetSystemLibrary,true,[]
      World Context Object,Input,Object,6FAB1F2E40E482619454AA842B390F56,,true,[]
      In String,Input,String,C21184844D66411D3D89EE8DFB5DD89B,Nice to meet you!!,true,[]
      Print to Screen,Input,Boolean,EC90BC1046D7AB1FBEB309A15DF87FA4,true,true,[]
      Print to Log,Input,Boolean,E1A51F0549680735E8DC549ECED33D09,true,true,[]
      Text Color,Input,Linear Color,B6D72FF44EF181599820E28881FC973A,(R=0.000000,G=1.000000,B=0.000000,A=1.000000),true,[]
      Duration,Input,Float (single-precision),A2CC654440E553AC90FA03ABB7F3E60B,10.000000,true,[]
      Key,Input,Name,F7F12F7846161B2188E38EB89836CD81,None,true,[]
  Print String,,false,
    pinDataList[10]{pinName,pinDirection,pinType,pinId,defaultValue,bThisPinUsesDefaultValue,connectedPinIds}:
      execute,Input,Exec,38A2D2F64DC6046FF9D271A4A76C63C8,,false,[7EE5C76349D1514966AF38BBB160EAC3]
      then,Output,Exec,9C0778834E4A81758E83FA834E91B2AD,,false,[]
      Target,Input,Kismet System Library,FF22BD2F4D6C374F30E125903EEA1C70,/Script/Engine.Default__KismetSystemLibrary,true,[]
      World Context Object,Input,Object,6FAB1F2E40E482619454AA842B390F56,,true,[]
      In String,Input,String,C21184844D66411D3D89EE8DFB5DD89B,We meet again!,true,[]
      Print to Screen,Input,Boolean,EC90BC1046D7AB1FBEB309A15DF87FA4,true,true,[]
      Print to Log,Input,Boolean,E1A51F0549680735E8DC549ECED33D09,true,true,[]
      Text Color,Input,Linear Color,B6D72FF44EF181599820E28881FC973A,(R=1.000000,G=0.000000,B=0.000000,A=1.000000),true,[]
      Duration,Input,Float (single-precision),A2CC654440E553AC90FA03ABB7F3E60B,10.000000,true,[]
      Key,Input,Name,F7F12F7846161B2188E38EB89836CD81,None,true,[]
```

</details>

In this example, the character count is 7,986 characters for JSON and 3,316 characters for TOON. (Using Unreal Engine’s standard copy function results in 23,278 characters.)

### Node Previewer
Displays a preview of the node you're hovering over in the node search window as a tooltip. Not only those defined within the engine, but also variables, functions, macros, custom events, and other user-defined items in blueprints and C++ can be displayed in the preview. To enable this feature, you need to check "Editor Preferences > Plugins > Tsubasamusu Unreal Assist > Node Previewer > Enable Node Preview".

> [!CAUTION]
> Some special nodes such as Documentation Node and attempting to preview nodes in assets other than normal blueprints and animation blueprints can cause crashes, so please use Node Previewer with caution.

<img width="700" src="https://github.com/user-attachments/assets/8acba045-cc94-4f53-ba49-95639f0ba065">

When you check "Enable Node Preview", the following properties can be edited:

| Property Name | Overview |
----|----
| Also Preview Advanced View | Whether to display pins that are collapsed by default in the preview. For example, properties like TextColor, Duration, and Key in the PrintString node. |
| Preview Scale | The scale of the node preview. The default value is 1.0. |

### Access Specifier Optimizer
When you enable this plugin and open the blueprint editor, an "Optimize Access Specifiers" menu is added to the "Edit" menu at the top of the editor. When you select this menu, a list of members defined in the blueprint that should have their access specifier changed is displayed. For example, members that are not set to Private despite being referenced only within that class, or members set to Public despite being referenced by other classes that are only derived classes of the base class. You can check the members whose access specifier you want to change and click the "Apply Optimal Access Specifiers" button to batch apply the optimal access specifier to those members.

> [!WARNING]
> - Custom event access specifiers are supported only in UE 5.3 and later. In UE 5.2 and earlier, custom events are not recognized as members of that class.
> - If there are unsaved or uncompiled blueprints, usage of those members cannot be correctly investigated, which may result in incorrect access specifier suggestions. It is recommended to save or compile all blueprints before using this feature.

<img width="500" src="https://github.com/user-attachments/assets/2f04d68e-bbda-4ecf-a65c-8c85fdd524f6">

### Access Specifier Initializer
Automatically changes the access specifier of a member when you add a new variable, function, or custom event to a blueprint. By default, the access specifier of members added in a blueprint is always Public, but by enabling this feature, you can automatically change it to Protected or Private when adding members.

The editor settings related to this feature are as follows:

| Property Name | Overview |
----|----
| Override Variable Default Access Specifier | Whether to override the default access specifier for new variables. |
| Variable Default Access Specifier | Default access specifier for new variables. (Private/Public) |
| Override Function Default Access Specifier | Whether to override the default access specifier for new functions. |
| Function Default Access Specifier | Default access specifier for new functions. (Private/Protected/Public) |
| Override Custom Event Default Access Specifier | Whether to override the default access specifier for new custom events. |
| Custom Event Default Access Specifier | Default access specifier for new custom events. (Private/Protected/Public) |

> [!WARNING]
> Custom event access specifiers are supported only in UE 5.3 and later. In UE 5.2 and earlier, "Override Custom Event Default Access Specifier" and "Custom Event Default Access Specifier" cannot be edited.

### Unused Function Deleter
When you enable this plugin and open the blueprint editor, a "Delete Unused Functions" menu is added to the "Edit" menu at the top of the editor. When you select this menu, a list of unused functions defined in the blueprint is displayed. You can check the functions you want to delete and click the "Delete Selected Functions" button to batch delete those functions.

> [!WARNING]
> Functions that are not newly declared in this blueprint, such as overridden functions or functions implemented in interfaces, may not correctly determine usage status and are therefore outside the scope of this feature.

<img width="500" src="https://github.com/user-attachments/assets/2c0f2ce5-8998-483f-b74d-1be53de84aca">

### Unused Local Variable Deleter
When you enable this plugin, open the blueprint editor, and focus on a function graph, a "Delete Unused Local Variables" menu is added to the "Edit" menu at the top of the editor. When you select this menu, a list of unused local variables defined in that function is displayed. You can check the local variables you want to delete and click the "Delete Selected Local Variables" button to batch delete those local variables.

<img width="500" src="https://github.com/user-attachments/assets/fa565e80-c798-4900-bcc7-fbb98d4686f0">

### Recommended Editor Settings Applier
When you enable this plugin, a property named "Apply Recommended Editor Settings" and a button labeled "Apply" appear in "Editor Preferences > Plugins > Tsubasamusu Unreal Assist > Recommended Editor Settings Applier". Clicking this button allows you to batch apply editor settings that [the developer of this plugin](https://x.com/tsubasamusu) personally recommends. The specific settings are as follows:

- Disable auto save.
- Do not restore the tabs of assets that were open previously when the editor is restarted.
- Set the location where asset editors open to “Main Window.”
- Make comment bubble visible when zooming out in the Event Graph.
- Set the default color of comment nodes to black.
- Set the editor language to English.
- Also use English for node names, pin names, and property names.
- Prevent nodes that are placed in a “ghost” state by default, such as BeginPlay and Tick, from being placed by default. (So that no “ghost” nodes exist when creating a Blueprint.)
- Make Cast nodes appear as Pure by default, with no execution pins.
- Display the Output Log in color.  
- When starting PIE, automatically focus the viewport even if it hasn’t been clicked."