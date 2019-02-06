#include "Writer.h"
#include "UI.h"

// mso.dll
#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" auto_rename no_namespace
// vbe6ext.olb
#import "libid:0002E157-0000-0000-C000-000000000046" auto_rename
// excel.exe
#import "libid:00020813-0000-0000-C000-000000000046" auto_rename

Writer::Writer()
{
}

Writer::~Writer()
{
}

void Writer::write(const std::vector<Lot>& data, const wchar_t * fileName)
{
	if (!fileName) throw std::exception("fileName was null at Writer::write.");

	Excel::_ApplicationPtr app;

	HRESULT hr = app.CreateInstance("Excel.Application");

	if (FAILED(hr)) throw std::exception("Failed to run Excel.");

	Excel::_WorkbookPtr book = nullptr;
	book = app->Workbooks->Add();

	if (!book) throw std::exception("Failed to create Excel book.");

	Excel::_WorksheetPtr sheet = nullptr;
	sheet = book->Sheets->Item[1];

	if (!sheet) throw std::exception("Failed to get Excel sheet.");

	book->SaveAs(_variant_t(fileName), Excel::xlOpenXMLWorkbook, vtMissing, vtMissing, vtMissing, vtMissing, Excel::xlShared);

	int currentRow = 3;

	sheet->Cells->Item[2][2] = L"№ п/п";
	sheet->Cells->Item[2][3] = L"Наименование";
	sheet->Cells->Item[2][4] = L"Количество";
	sheet->Cells->Item[2][5] = L"Цена";

	UI & ui = UI::getInstance();

	for (auto it = data.begin(); it != data.end(); it++)
	{
		Lot lot = *it;

		sheet->Cells->Item[currentRow][2] = currentRow - 2;
		sheet->Cells->Item[currentRow][3] = lot.getName().c_str();
		sheet->Cells->Item[currentRow][4] = lot.getInStock();
		sheet->Cells->Item[currentRow][5] = lot.getPrice();

		ui.updateStatus(currentRow - 2);

		currentRow++;
	}

	sheet->Columns->AutoFit();

	book->SaveAs(_variant_t(fileName), Excel::xlOpenXMLWorkbook, vtMissing, vtMissing, vtMissing, vtMissing, Excel::xlShared);
	book->Close();
	app->Quit();
}
