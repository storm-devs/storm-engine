// ������ �� �������
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat("��� ������� ������ "+ GetCityName(npchar.city) +" � ����� �������. ��� �� �� ������ ������?",
                          "�� ������ ��� ��������� ��� ����. ��, ��������, ������������...", "������� �� ��� ������ ��� �������� � �����-�� �������...",
                          "��� �� ������� ��� ������� ���� � �� ��...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("������, " + NPChar.name + ", ���-������ � ��������� ���.", "�����, ����� ���-��...",
                      "�� ��, ������������� � ������ ���...", "�� ��...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
            // ==> ������ ����� ��������, ������ �������
            if (pchar.questTemp.State == "MorrisWillams_ArrestedSeekInfo")
			{
                link.l1 = "��������, � ��� �� ������� ������� ���������?";
                link.l1.go = "Step_E6_1";
			}
            // ==> ������� ����� ��.�������, ���������� ���� ��������
            if (pchar.questTemp.State == "Fr8ThreeCorsairs_toSeek")
			{
                link.l1 = "��� � ���� ����� �������� ���? � ���� � ���� ������ ����.";
                link.l1.go = "Step_F8_1";
			}
		break;
        //********************** ���������� ������� **********************
        case "Step_E6_1":
			dialog.text = "��������� �� � ���� ���������, ������� ����� ����� ��� ���.";
			link.l1 = "���-������ ���������� ������ � ���?";
			link.l1.go = "Step_E6_2";
		break;
        case "Step_E6_2":
			dialog.text = "�� ���, ������ ����������.";
			link.l1 = "� ��� � ������ � ��� ������ ������?";
			link.l1.go = "Step_E6_3";
		break;
        case "Step_E6_3":
			dialog.text = "�� �� � ��� �� ������ �� ��������, ������ �� ���� �������.";
			link.l1 = "�� ��� �, ������� � �� ����.";
			link.l1.go = "exit";
			pchar.questTemp.State = "MorrisWillams_ArrestedSeekInfoFather";
		break;
		//********************** ����������� ������� **********************
        case "Step_F8_1":
			dialog.text = NPCStringReactionRepeat("�������� ���? ��� ������ �����. �� ������� � ���� �������, � ������� ��� �� �������, ��� ��� �� ��� ��� ����������� �������.", 
				          "� ��� ��� ������...", "�� � ��� ��������...", "���, ����� ��� ����������� ���������� ������ ������!", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("�������, �������...", "�����, ������...", "��, �� ��...", "��, ��, ������, ��������...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
        break;



	}
	UnloadSegment(NPChar.FileDialog2);  // ���� ���-�� ����� ������ switch  �� return �� ������ ������� �����
}

