from django.db import models
from django.utils import timezone


STATES = [
    (0, "ONLINE"),
    (1, "OFFLINE"),
    (2, "ERROR"),
    (3, "DEACTIVATED"),
    (4, "MISSING_SDCARD")
]

# Create your models here.
class Camera(models.Model):
    id = models.AutoField(primary_key=True)
    ip4 = models.CharField(max_length=16, unique=True)
    state = models.CharField(max_length=15,
                             choices=STATES,
                             default=1)
    interval = models.PositiveIntegerField(default=10000)
    quality = models.PositiveIntegerField(default=10)
    last_active = models.DateTimeField(default=timezone.now)


class Picture(models.Model):
    id = models.AutoField(primary_key=True)
    filename = models.CharField(max_length=64)
    camera = models.ForeignKey(Camera, on_delete=models.CASCADE)
    picture = models.ImageField(blank=True)
