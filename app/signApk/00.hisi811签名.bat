:begin
set/p a1=������APK���Ʋ����س���
:next
java -jar signapk811.jar platform811.x509.pem platform811.pk8 %a1%.apk %a1%_sign.apk
