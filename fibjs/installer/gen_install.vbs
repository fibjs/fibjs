Dim build, fs, shell

Set fs = CreateObject("Scripting.FileSystemObject")
Set shell = CreateObject("WSCript.shell")

Dim st1, st2, st3

Set st1 = fs.GetFile("fibjs.exe")

If (Not fs.FileExists("fibjs.cab")) Then
    build = True
Else
    Set st2 = fs.GetFile("fibjs.cab")
    if st2.DateLastModified < st1.DateLastModified Then
        build = True
    End If
End If

If build  Then
    WScript.echo "Compressing fibjs.exe to fibjs.cab........."
    shell.Run fs.GetSpecialFolder(1) + "\makecab fibjs.exe fibjs.cab", 0, true

    Set st2 = fs.GetFile("fibjs.cab")
    WScript.echo ''
End If

build = False
If Not fs.FileExists("js.h") Then
    build = True
Else
    Set st3 = fs.GetFile("js.h")
    if st3.DateLastModified < st2.DateLastModified Then
        build = True
    End If
End If

Dim bs, ts
Dim data, len, b, s

If build Then
    WScript.echo "Encoding fibjs.cab to js.h........."

    Set bs = WScript.CreateObject("ADODB.Stream")
    bs.Type = 1
    bs.Open

    bs.LoadFromFile "fibjs.cab"
    len = bs.Size

    Set ts = WScript.CreateObject("ADODB.Stream")
    ts.Type = 2
    ts.Open

    ts.WriteText "unsigned char js_data[] = {"
    For i = 1 To len
        if i Mod 32 = 0 Then
            ts.WriteText vbCRLF
        End If

        b = bs.read(1)
        s = CStr(AscB(b))
        If i < len Then
            ts.WriteText s + ", "
        Else
            ts.WriteText s
        End If

        if i Mod 1000 = 0 Then
            WScript.StdOut.Write vbCR + "processed: " + CStr(i) + " bytes."
        End If
    Next
    ts.WriteText "};" + vbCRLF

    WScript.echo "saving to js.h..."
    ts.SaveToFile "js.h", 2
    ts.Close
End If

WScript.echo ""
