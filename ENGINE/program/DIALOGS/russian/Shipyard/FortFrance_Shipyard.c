// ������ �� �������
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("����� �������?", "��� ��� ������?"), "������ ������� �� �������� ������ ��� ������...", "� � ���� �� �����, � � ������ � ������, � ����� ����������� �������������� �� �����.",
                          "�� ��� �� �������? ��� ���� - ������� �������, ������� ���� � ��������.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("� ���������...", "������ ��� �� � ��� ��������"), "��, ���-�� � ������� ���� �����...",
                      "��, ������...", "�������...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (pchar.questTemp.piratesLine == "KillLoy_toSeek")
			{
				link.l1 = "� ��� ��������� ����� ������ ���, � ����� ������������ �������?";
				link.l1.go = "PL_Q3_1";
			}
		break;
		//�������, ����� �3, ������ ���
		case "PL_Q3_1":
			dialog.text = NPCStringReactionRepeat("��������, ������, �� �������� ������.", 
				"� ��� ��� ������ �� ����� ������...", 
				"���������, ���������. ������ � ���������� ����� ��� �������� ������.",
                "��������, ������!", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("��� ���?", 
				"��, �����...",
                "�����.", 
				"��� �����...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("PL_Q3_2", "", "", "", npchar, Dialog.CurrentNode);
		break;
		case "PL_Q3_2":
			dialog.text = "�� �� ���� �����, � �����, � ���������, � ���� ������-������ ��  ������ ����.";
			link.l1 = "����... �� ��������, ���� �� �����?";
			link.l1.go = "PL_Q3_3";
		break;
		case "PL_Q3_3":
			dialog.text = "��-��, �������� ����. � ���������� �����, ����� ������, ������ ��� ��� ����� �� ���, ��-��-��...";
			link.l1 = "� ���� ��� �� ��� ������?";
			link.l1.go = "PL_Q3_4";
		break;
		case "PL_Q3_4":
			dialog.text = "��� �� ������� �� ��������, ����� ����, ����� �� �����, ��-��...";
			link.l1 = "�������!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "4");
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // ���� ���-�� ����� ������ switch  �� return �� ������ ������� �����
}

