unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ShellAPI, LibXL;

type

  { TForm1 }

  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
  private

  public

  end;

var
  Form1: TForm1;

implementation

{$R *.lfm}

{ TForm1 }

procedure TForm1.Button1Click(Sender: TObject);
var
  Book: TXLBook;
  Sheet: TXLSheet;
  dateFormat: TXLFormat;
begin
  Book := TXmlBook.Create;
  Sheet := Book.addSheet('Sheet1');

  Sheet.writeStr(2, 1, 'Hello, World !');
  Sheet.writeNum(3, 1, 1000);

  dateFormat := Book.addFormat();
  dateFormat.setNumFormat(NUMFORMAT_DATE);
  Sheet.writeNum(4, 1, Book.datePack(2008, 4, 29), dateFormat);
  Sheet.setCol(1, 1, 12);

  Book.save('example.xlsx');
  ShellExecute(0, 'open', 'example.xlsx', nil, nil, 5);
  Book.Free;
end;

procedure TForm1.Button2Click(Sender: TObject);
var
  Book: TXLBook;
  Sheet: TXLSheet;
  d: double;
  s: string;
begin
  Book := TXmlBook.Create;
  Book.load('example.xlsx');
  Sheet := Book.getSheet(0);

  s := string(Sheet.readStr(2, 1));
  d := Sheet.readNum(3, 1);

  ShowMessage(s + #13#10 + FloatToStr(d));

  Book.Free;
end;

procedure TForm1.Button3Click(Sender: TObject);
var
  Book: TXLBook;
  Sheet: TXLSheet;
  d: double;
begin
  Book := TXmlBook.Create;
  Book.load('example.xlsx');
  Sheet := Book.getSheet(0);

  d := Sheet.readNum(3, 1);

  Sheet.writeNum(3, 1, d * 2);

  Book.save('example.xlsx');
  ShellExecute(0, 'open', 'example.xlsx', nil, nil, 5);
  Book.Free;
end;

procedure TForm1.Button4Click(Sender: TObject);
var
  Book: TXLBook;
  Sheet: TXLSheet;
  Fnt: TXLFont;
  Format: TXLFormat;
begin
  Book := TXmlBook.Create;

  Fnt := Book.addFont;
  Fnt.size := 36;

  Format := Book.addFormat();
  Format.alignH := ALIGNH_CENTER;
  Format.setBorder(BORDERSTYLE_MEDIUMDASHDOTDOT);
  Format.setBorderColor(COLOR_RED);
  Format.font := Fnt;

  Sheet := Book.addSheet('Sheet1');
  Sheet.writeStr(2, 1, 'Format', Format);
  Sheet.setCol(1, 1, 25);

  Book.save('format.xlsx');
  ShellExecute(0, 'open', 'format.xlsx', nil, nil, 5);
  Book.Free;
end;

procedure TForm1.Button5Click(Sender: TObject);
var
  Book: TXLBook;
  Sheet: TXLSheet;
  f: array[0..5] of Integer;
  format: array[0..5] of TXLFormat;
  i: Integer;
begin
  Book := TXmlBook.Create;

  f[0] := Book.addCustomNumFormat('0.0');
  f[1] := Book.addCustomNumFormat('0.00');
  f[2] := Book.addCustomNumFormat('0.000');
  f[3] := Book.addCustomNumFormat('0.0000');
  f[4] := Book.addCustomNumFormat('#,###.00 $');
  f[5] := Book.addCustomNumFormat('#,###.00 $[Black][<1000];#,###.00 $[Red][>=1000]');

  for i := 0 to 5 do
  begin
    format[i] := Book.addFormat();
    format[i].setNumFormat(f[i]);
  end;

  Sheet := Book.addSheet('Custom formats');

  Sheet.setCol(0, 0, 20);

  Sheet.writeNum(2, 0, 25.718, format[0]);
  Sheet.writeNum(3, 0, 25.718, format[1]);
  Sheet.writeNum(4, 0, 25.718, format[2]);
  Sheet.writeNum(5, 0, 25.718, format[3]);

  Sheet.writeNum(7, 0, 1800.5, format[4]);

  Sheet.writeNum(9, 0, 500, format[5]);
  Sheet.writeNum(10, 0, 1600, format[5]);

  Book.save('custom.xlsx');
  ShellExecute(0, 'open', 'custom.xlsx', nil, nil, 5);
  Book.Free;
end;

procedure TForm1.Button6Click(Sender: TObject);
var
  Book: TXLBook;
  boldFont, titleFont: TXLFont;
  titleFormat, headerFormat, descriptionFormat, amountFormat, totalLabelFormat, totalFormat, signatureFormat: TXLFormat;
  Sheet: TXLSheet;
begin
  Book := TXmlBook.Create;
  boldFont := Book.addFont();
  boldFont.bold := true;

  titleFont := Book.addFont();
  titleFont.name := 'Arial Black';
  titleFont.size := 16;

  titleFormat := Book.addFormat();
  titleFormat.font := titleFont;

  headerFormat := Book.addFormat();
  headerFormat.alignH := ALIGNH_CENTER;
  headerFormat.setBorder(BORDERSTYLE_THIN);
  headerFormat.font := boldFont;
  headerFormat.fillPattern := FILLPATTERN_SOLID;
  headerFormat.patternForegroundColor := COLOR_TAN;

  descriptionFormat := Book.addFormat();
  descriptionFormat.borderLeft := BORDERSTYLE_THIN;

  amountFormat := Book.addFormat();
  amountFormat.setNumFormat(NUMFORMAT_CURRENCY_NEGBRA);
  amountFormat.borderLeft := BORDERSTYLE_THIN;
  amountFormat.borderRight := BORDERSTYLE_THIN;

  totalLabelFormat := Book.addFormat();
  totalLabelFormat.borderTop := BORDERSTYLE_THIN;
  totalLabelFormat.alignH := ALIGNH_RIGHT;
  totalLabelFormat.font := boldFont;

  totalFormat := Book.addFormat();
  totalFormat.setNumFormat(NUMFORMAT_CURRENCY_NEGBRA);
  totalFormat.setBorder(BORDERSTYLE_THIN);
  totalFormat.font := boldFont;
  totalFormat.fillPattern := FILLPATTERN_SOLID;
  totalFormat.patternForegroundColor := COLOR_YELLOW;

  signatureFormat := Book.addFormat();
  signatureFormat.alignH := ALIGNH_CENTER;
  signatureFormat.borderTop := BORDERSTYLE_THIN;

  Sheet := Book.addSheet('Invoice');

  Sheet.writeStr(2, 1, 'Invoice No.3568', titleFormat);

  Sheet.writeStr(4, 1, 'Name: John Smith');
  Sheet.writeStr(5, 1, 'Address: San Ramon, CA 94583 USA');

  Sheet.writeStr(7, 1, 'Description', headerFormat);
  Sheet.writeStr(7, 2, 'Amount', headerFormat);

  Sheet.writeStr(8, 1, 'Ball - Point Pens', descriptionFormat);
  Sheet.writeNum(8, 2, 85, amountFormat);
  Sheet.writeStr(9, 1, 'T - Shirts', descriptionFormat);
  Sheet.writeNum(9, 2, 150, amountFormat);
  Sheet.writeStr(10, 1, 'Tea cups', descriptionFormat);
  Sheet.writeNum(10, 2, 45, amountFormat);

  Sheet.writeStr(11, 1, 'Total:', totalLabelFormat);
  Sheet.writeFormula(11, 2, '=SUM(C9:C11)', totalFormat);

  Sheet.writeStr(14, 2, 'Signature', signatureFormat);

  Sheet.setCol(1, 1, 40);
  Sheet.setCol(2, 2, 15);

  Book.save('invoice.xlsx');
  ShellExecute(0, 'open', 'invoice.xlsx', nil, nil, 5);
  Book.Free;
end;

end.

