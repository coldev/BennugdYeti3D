unit Yeti3D;

interface

uses Math, Classes, SysUtils;

function RGB555(R, G, B: Single): Integer;
function Gamma(const AShade: Single): Single;
procedure GetListOfFileNames(const SL: TStrings; const AFilter: String);

implementation

function RGB555(R, G, B: Single): Integer;
begin
  R := EnsureRange(R, 0, 255) * 31 / 255;
  G := EnsureRange(G, 0, 255) * 31 / 255;
  B := EnsureRange(B, 0, 255) * 31 / 255;
  Result := (Trunc(B) shl 10) or (Trunc(G) shl 5) or Trunc(R);
end;

function Gamma(const AShade: Single): Single;
begin
  //Result := AShade * 0.5 + 128.0;
  Result := Power(AShade / 255.0, 1.0 / 2.0) * 255.0;
  //Result := AShade;
end;

procedure GetListOfFileNames(const SL: TStrings; const AFilter: String);
var SR: TSearchRec;
begin
  if FindFirst(AFilter, faAnyFile, SR) = 0 then
  try
    repeat
      SL.Add(ExtractFilePath(AFilter) + SR.Name)
    until FindNext(SR) <> 0;
  finally
    FindClose(SR);
  end;
end;


end.
