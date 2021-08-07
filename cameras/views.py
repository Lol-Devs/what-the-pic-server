import base64

import requests
from rest_framework import status
from rest_framework.response import Response
from rest_framework.views import APIView
from django.http import HttpResponse, Http404
from cameras.models import Camera, Picture


# Create your views here.
class CameraList(APIView):
    def get(self, request):
        cameras = Camera.objects.all()
        ret = []
        for camera in cameras:
            ret.append(CameraDetail.get_camera(None, camera.id))
        return Response(ret)

    def post(self, request):
        if "ip4" not in request.data:
            return Response("IPv4 Address is required", status=status.HTTP_400_BAD_REQUEST)

        try:
            c = Camera.objects.create(ip4=request.data["ip4"])
        except:
            return Response("Invalid data", status=status.HTTP_400_BAD_REQUEST)
        return Response(CameraDetail.get_camera(None, id=c.id))


class CameraDetail(APIView):
    def get_camera(self, id):
        try:
            camera = Camera.objects.get(id=id)
            pictures = Picture.objects.filter(camera_id=id)

            import base64
            pics = []

            for pic in pictures:
                base64.b64encode(pic.tobytes())
                pics.append({
                    "filename":pic.filename,
                    "picture":pic.image,
                })

            ret = {
                "id": camera.id,
                "ip4": camera.ip4,
                "interval": camera.interval,
                "quality": camera.quality,
                "last_active": camera.last_active,
                "pictures": pics
            }
            return ret
        except Camera.DoesNotExist:
            raise Http404

    def get(self, request, id):
        ret = self.get_camera(id)
        return Response(ret)

    def post(self, request, id):
        camera = self.get_camera(id)
        try:
            resp = requests.post("http://" + camera["ip4"] + "/takePictureNow")#
            if resp.status_code == 200:
                return Response()
            return Response(status=status.HTTP_400_BAD_REQUEST)
        except:
            return Response(status=status.HTTP_429_TOO_MANY_REQUESTS)

    def put(self, request, id):
        if "interval" not in request.data:
            return Response("Interval is required", status=status.HTTP_400_BAD_REQUEST)
        if request.data["interval"] <= 0:
            return Response("Interval must be greater than 0", status=status.HTTP_400_BAD_REQUEST)

        if "quality" not in request.data:
            return Response("Quality is required", status=status.HTTP_400_BAD_REQUEST)
        if request.data["quality"] < 10 or request.data["quality"] > 63:
            return Response("Quality must be between 10 and 63", status=status.HTTP_400_BAD_REQUEST)

        camera = self.get_camera(id)
        try:
            resp = requests.post("http://" + camera["ip4"] + "/config", request.data)
        except:
            return Response(status=status.HTTP_400_BAD_REQUEST)

        if resp.status_code == 200:
            camera = Camera.objects.get(id=id)
            camera.interval = request.data["interval"]
            camera.quality = request.data["quality"]
            camera.save()
            return Response(status=status.HTTP_200_OK)

        return Response(status=status.HTTP_400_BAD_REQUEST)


class PictureView(APIView):
    def post(self, request, ip4):
        try:
            camera = Camera.objects.get(ip4=ip4)
        except Camera.DoesNotExist:
            return Response(status=status.HTTP_400_BAD_REQUEST)

        if request.FILES['imageFile']:
            image_file_obj = request.FILES['imageFile']
            filename = request.FILES['imageFile'].name
            Picture.objects.create(camera_id=camera.id,
                                   picture=image_file_obj,
                                   filename=filename)
            return Response()
        else:
            return Response(dict(error="no image uploaded"), status_code=status.HTTP_400_BAD_REQUEST)
