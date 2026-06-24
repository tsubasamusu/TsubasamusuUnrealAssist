# Tsubasamusu Unreal Assist（Experimental）
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tsubasamusu/TsubasamusuUnrealAssist)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![UE Version](https://img.shields.io/badge/Unreal%20Engine-5.0--5.8-0e1128?logo=unrealengine&logoColor=white)
[![X (formerly Twitter) Follow](https://img.shields.io/twitter/follow/tsubasamusu?style=social)](https://twitter.com/tsubasamusu)

## 概要
UE5 に個人的に欲しいと思っている機能や、SNSで見かけた「こういう機能があったらいいのに…」といったものをひたすら追加していく Unreal Editor の拡張プラグインです。このプラグインを有効化すると「Editor Preferences ＞ Plugins」に「Tsubasamusu Unreal Assist」というカテゴリが追加され、そこでこのプラグイン関連の設定を変更することができます。

> [!CAUTION]
> このプラグインは実験的機能（Experimental）であり、[MITライセンス](https://opensource.org/license/mit)で公開しています。このプラグインを使用したことによって生じたいかなる問題に対しても責任を負いません。

> [!WARNING]
> 現在サポートしているプラットフォームは Win64 のみです。

## エンジンバージョン
サポート対象のエンジンバージョンは以下の通りです。

- 5.0
- 5.1
- 5.2
- 5.3
- 5.4
- 5.5
- 5.6
- 5.7
- 5.8

## 導入手順
### プラグインのインストール
1. [最新版のリリース](https://github.com/tsubasamusu/TsubasamusuUnrealAssist/releases/latest)の「Source code (zip)」をダウンロードします。
2. ダウンロードしたZIPファイルをエンジンまたはUEプロジェクトの Plugins フォルダに展開します。
3. UEプロジェクトを開いて「Edit ＞ Plugins」の「Editor」カテゴリにある「Tsubasamusu Unreal Assist」にチェックを付けてエディタを再起動します。
4. 必要に応じてIDEからビルドします。
5. 「Editor Preferences ＞ Plugins」に「Tsubasamusu Unreal Assist」というカテゴリが追加されていればOKです。

### ローカルLLMの使用
現在、このプラグインではコメント生成機能（Comment Generator）でローカルLLMを使用しています。ローカルLLMの使用にあたっては、ローカル環境でLLMを動作させるための軽量な実行エンジンである [llama.cpp](https://github.com/ggml-org/llama.cpp) で提供されている「LLaMA.cpp HTTP Server」を利用しています。

このプラグインでのセットアップ手順は以下の通りです。

> [!WARNING]
> このプラグインのサポート対象のプラットフォームは Win64 のみなので、以下の手順は Win64 環境でのものです。

1. [llama.cpp の最新版のリリース](https://github.com/ggml-org/llama.cpp/releases/latest) の「llama-{ビルド番号}-bin-win-{種類}-x64.zip」をダウンロードします。推奨は「llama-b8475-bin-win-**cpu**-x64.zip」と「llama-b8475-bin-win-**vulkan**-x64.zip」ですが、ご自身の環境に合わせたものをダウンロードしてください。
2. ダウンロードしたZIPファイルを展開します。
3. Unreal Editor を開き、展開したファイルの中に含まれている「llama-server.exe」のパスを「Editor Preferences ＞ Plugins ＞ Tsubasamusu Unreal Assist ＞ LLM ＞ Llama Server File Path」に設定します。
4. 使用したいモデルのGGUFファイルを [Hugging Face](https://huggingface.co) からダウンロードします。
5. 「Editor Preferences ＞ Plugins ＞ Tsubasamusu Unreal Assist ＞ LLM ＞ Llama Server Options」に「Model」オプションを追加して、ダウンロードしたGGUFファイルのパスを「LLM File Path」に設定します。ここで他のオプションもいくつか設定することができますが、最低限「Model」オプションのみ設定されていればローカルLLMを使用することができます。
6. 警告メッセージとともに「Start Llama Server」というボタンが表示されるのでそれを押します。「Llama Server File Path」や「LLM File Path」が正しく設定されているとサーバーの起動に成功し、この警告メッセージが表示されなくなります。これでローカルLLMを使用できる状態になりました。

<img width="700" src="https://github.com/user-attachments/assets/8d1940d9-c609-4ebb-bbe3-4949ee3d4010">

## 機能一覧
### Comment Generator
その中に他のノードを1つ以上含んでいるコメントノードを右クリックすると「Generate Comment」というメニューが表示されます。このメニューをクリックするとローカルLLMを使用してコメントを生成することができます。

<img width="700" src="https://github.com/user-attachments/assets/fd577e3e-6ea9-4047-811e-67e29e412071">

この機能に関連したエディタ設定の項目は以下の通りです。

| プロパティ名 | 概要 |
----|----
| Use Editor Language | 生成するコメントの言語をエディタ言語と同じにするかどうか。 |
| Language | 生成するコメントの言語。このプロパティは「Use Editor Language」のチェックを外すと編集できるようになります。逆に「Use Editor Language」にチェックを付けるとこのプロパティの値はエディタ言語と同じになります。※使用するLLMによってはこのプロパティで設定した言語通りのコメントを生成してくれないこともあります。 |
| Enable Streaming Generation | コメントのストリーミング生成を有効にするかどうか。このプロパティにチェックを付けると ChatGPT や Claude、Gemini などのチャット画面のようにコメントがトークンごとに生成されて反映されます。このプロパティのチェックを外すとコメントの生成完了後に1度だけコメントを更新します。 |
| Ignore Nodes Do Not Have Connected Pins | コメントを生成するときにインプットピンやアウトプットピン、実行ピンなどが他のピンと1つも接続されていないノードを無視するかどうか。 |
| Ignore Comment Nodes | コメントを生成するときにそのコメントノードの中にある別のコメントノードを無視するかどうか。 |
| Use TOON Format | コメントノードの中にあるノードの情報をLLMに渡す際、それを[TOON](https://toonformat.dev)形式の文字列として渡すかどうか。このプロパティのチェックを外すとTOONではなく、JSONが使用されます。TOONの方がJSONよりもトークン数を抑えることができます。 |
| Conditions | LLMがコメントを生成するときに従う条件。例えば「そのブループリントで行っていることを簡潔にまとめること」や「だ・である口調で生成すること」などです。 |

### Comment Translator
コメントが設定されているコメントノードを右クリックすると「Translate to...」というメニューが表示されます。さらにそのメニューをマウスホバーすると翻訳先の言語の一覧が表示されます。その中のいずれかの言語を選択すると、コメントノードに書かれているコメントをその言語に翻訳することができます。

> [!NOTE]
> この機能を使用するにはエディタ設定の「Comment Translator > DeepL API Key」で事前にAPIキーを設定しておく必要があります。

<img width="500" src="https://github.com/user-attachments/assets/1f25d9e4-5037-42c7-b561-46e56d176f39">

### Array Node Creator
同じ型のアウトプットピンを持つ複数のノードを選択している状態で右クリックすると「Make Array」というメニューが表示されます。このメニューを選択すると、それらのアウトプットピンが接続された Make Array ノードを一瞬で作成し、グラフに追加することができます。

<img width="500" src="https://github.com/user-attachments/assets/1b7cf903-4eba-44a6-b475-5e513eb4b4d5">

### Node Information Copier
ノードを1つ以上選択している状態で右クリックすると「Copy as...」というメニューが表示されます。さらにそのメニューをマウスホバーすると「JSON」と「TOON」という2つの項目が表示されます。どちらかを選択すると、それらのノードの情報をJSONまたは[TOON](https://toonformat.dev)形式の文字列としてクリップボードにコピーすることができます。この文字列にはノード名やピン名、ノードコメント、ピンの方向（インプットかアウトプット）、ピンの型名、ピンのGUID、そのピンに接続されているピンのGUIDなどが含まれます。「JSON」を選択すると、Unreal Engine の標準のコピー機能でクリップボードにコピーされる文字数のおよそ3分の1ほどの文字数でコピーすることができます。さらに「TOON」を選択すると、JSONのときの半分ほどの文字数でコピーすることができます。

<img width="700" src="https://github.com/user-attachments/assets/7a4b25c9-46c1-4b39-a6b6-09251ca1801b">

例えば上の画像に写っているブループリントは「L_Sample」というレベルのレベルブループリントであり、「IsFirstPlay」という bool 型の変数の値に応じて「Nice to meet you!!」か「We meet again!」というテキストを画面に表示しています。このプラグインのコピー機能を使用してコピーした実際の文字列は以下の通りです。

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

この例ではJSONのときの文字数が7,986文字、TOONのときは3,316文字になっています。（Unreal Engine の標準のコピー機能では23,278文字になります。）

### Node Previewer
ノード検索ウィンドウでマウスホバーしているノードのプレビューをツールチップに表示します。既にエンジン内部で定義されているものに限らず、ブループリントやC++でユーザーが独自に定義した変数や関数、マクロ、カスタムイベントなどもプレビューに表示できます。この機能を有効化するには「Editor Preferences > Plugins > Tsubasamusu Unreal Assist > Node Previewer > Enable Node Preview」にチェックを付ける必要があります。

> [!CAUTION]
> Documentation Node などの一部の特殊なノードや、通常のブループリントやアニメーションブループリント以外のアセットでノードをプレビューしようとするとクラッシュしてしまうことがあるため、Node Previewer の使用は慎重にお願いします。

<img width="700" src="https://github.com/user-attachments/assets/8acba045-cc94-4f53-ba49-95639f0ba065">

「Enable Node Preview」にチェックを付けると、以下のプロパティが編集できるようになります。

| プロパティ名 | 概要 |
----|----
| Also Preview Advanced View  | デフォルトで折り畳まれているピンをプレビューに表示するかどうか。例えば PrintString ノードの TextColor や Duration、Key などがそれにあたります。 |
| Preview Scale  | ノードプレビューのスケール。初期値は1.0です。 |

### Access Specifier Optimizer
このプラグインを有効化してブループリントエディタを開くと、「Optimize Access Specifiers」というメニューがエディタ上部の「Edit」に追加されます。このメニューを選択すると、そのブループリントで定義されているメンバの中でアクセス修飾子を変更した方がいいものが一覧で表示されます。例えば、そのクラス内でしか参照されていないにも関わらず、アクセス修飾子が Private になっていないものや、他のクラスから参照されていてもそれらのクラスが基のクラスの派生クラスしかないにも関わらず、アクセス修飾子が Public になっているものなどです。アクセス修飾子を変更したいメンバにチェックを付けて「Apply Optimal Access Specifiers」というボタンを押すと、それらのメンバに最適なアクセス修飾子を一括で適用することができます。

> [!WARNING]
> - カスタムイベントのアクセス修飾子は UE 5.3 以降でのみ対応しています。UE 5.2 以前ではカスタムイベントがそのクラスのメンバとして認識されないようになっています。
> - 保存またはコンパイルしていないブループリントがある状態ではそのメンバの使用箇所を正しく調査できず、誤ったアクセス修飾子を提案してしまうことがあります。この機能を使用する前に全てのブループリントを保存またはコンパイルしておくことを推奨します。

<img width="500" src="https://github.com/user-attachments/assets/2f04d68e-bbda-4ecf-a65c-8c85fdd524f6">

### Access Specifier Initializer
ブループリントに新しい変数や関数、カスタムイベントを追加した際にそのメンバのアクセス修飾子を自動的に変更します。デフォルトでは、ブループリントで追加したメンバのアクセス修飾子は常に Public になっていますが、この機能を有効化することでメンバの追加時に自動的に Protected や Private などに変更することができます。

この機能に関連したエディタ設定の項目は以下の通りです。

| プロパティ名 | 概要 |
----|----
| Override Variable Default Access Specifier | 新規変数のデフォルトアクセス修飾子を上書きするかどうか。 |
| Variable Default Access Specifier | 新規変数のデフォルトアクセス修飾子。（Private・Public） |
| Override Function Default Access Specifier | 新規関数のデフォルトアクセス修飾子を上書きするかどうか。 |
| Function Default Access Specifier | 新規関数のデフォルトアクセス修飾子。（Private・Protected・Public） |
| Override Custom Event Default Access Specifier | 新規カスタムイベントのデフォルトアクセス修飾子を上書きするかどうか。 |
| Custom Event Default Access Specifier | 新規カスタムイベントのデフォルトアクセス修飾子。（Private・Protected・Public） |

> [!WARNING]
> カスタムイベントのアクセス修飾子は UE 5.3 以降でのみ対応しています。UE 5.2 以前では「Override Custom Event Default Access Specifier」と「Custom Event Default Access Specifier」は編集できません。

### Unused Function Deleter
このプラグインを有効化してブループリントエディタを開くと「Delete Unused Functions」というメニューがエディタ上部の「Edit」に追加されます。このメニューを選択すると、そのブループリントで定義されている関数の中で未使用のものが一覧で表示されます。削除したい関数にチェックを付けて「Delete Selected Functions」というボタンを押すと、それらの関数を一括で削除することができます。

> [!WARNING]
> オーバーライドした関数やインターフェースで実装した関数などの、そのブループリントで新たに宣言されたわけではない関数は使用状態を正しく判定できない可能性があるため、この機能の対象外です。

<img width="500" src="https://github.com/user-attachments/assets/2c0f2ce5-8998-483f-b74d-1be53de84aca">

### Unused Local Variable Deleter
このプラグインを有効化してブループリントエディタを開き、関数グラフにフォーカスすると「Delete Unused Local Variables」というメニューがエディタ上部の「Edit」に追加されます。このメニューを選択すると、その関数で定義されているローカル変数の中で未使用のものが一覧で表示されます。削除したいローカル変数にチェックを付けて「Delete Selected Local Variables」というボタンを押すと、それらのローカル変数を一括で削除することができます。

<img width="500" src="https://github.com/user-attachments/assets/fa565e80-c798-4900-bcc7-fbb98d4686f0">

### Recommended Editor Settings Applier
このプラグインを有効化すると「Apply Recommended Editor Settings」という名前のプロパティと、「Apply」と書かれたボタンが「Editor Preferences ＞ Plugins ＞ Tsubasamusu Unreal Assist ＞ Recommended Editor Settings Applier」に表示されます。このボタンをクリックすると、[このプラグインの開発者](https://x.com/tsubasamusu)が個人的に推奨しているエディタ設定を一括で適用することができます。具体的な設定内容は以下の通りです。

- 自動保存を無効化します。
- 前回開いていたアセットエディタが復元されないようにします。
- アセットエディタがメインウィンドウで開くようにします。
- ブループリントグラフでズームアウトしたときに Comment Bubble が表示されるようにします。
- コメントノードのデフォルトカラーを黒色に変更します。
- エディタ言語を英語に変更します。ノード名やピン名、プロパティ名などの言語も英語になります。
- BeginPlay や Tick などの、ブループリントを作成したときにゴースト状態で初めから配置されているノードが配置されないようにします。
- Cast ノードが実行ピンのない、ピュアな状態で出現するようにします。
- アウトプットログがカラフルに色付けされるようにします。
- PIE を開始したときにビューポートをクリックしなくても自動的にビューポートにフォーカスされるようにします。