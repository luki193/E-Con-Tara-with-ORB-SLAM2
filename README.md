# E-Con-Tara-with-ORB-SLAM2
Create 3D vision using stereo camera with ORB-SLAM2


### Uruchomienie
Folder see3cam należy skopiować do Workspace/

Następnie w konsoli:

```
$ source devel/setup.bash
& roslaunch uvc_camera tara_launch.launch
```


### Zmiany względem oryginału
W celu kompatybilności ORB-SLAM2 z kamerą podmieniono nazwy plików wykonywalnych z stereo.cpp na camera.cpp. Ponieważ plik wykonywanly o takiej nazwie już istniał nie były konieczne zmiany w CMake. Dodatkowo w roslaunch należało zmienić etykiety z stereo na camera. Dzięki tym zmianą nazwy topiców zgadzają się z stosowanymi w ORB-SLAM2.

W przypadku korzystania z urządzenia posiadającego wbudowaną kamerę należy w pliku roslaunch również zmodyfikować nazwę urządzenia z dev0 na dev1
