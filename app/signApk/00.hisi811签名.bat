:begin
set/p a1=请输入APK名称并按回车：
:next
java -jar signapk811.jar platform811.x509.pem platform811.pk8 %a1%.apk %a1%_sign.apk
