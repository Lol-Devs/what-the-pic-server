from django.urls import path
from django.conf.urls import url

from . import views

urlpatterns = [
    path('camera/', views.CameraList.as_view()),
    path('camera/<int:id>', views.CameraDetail.as_view()),
    path('picture/<str:ip4>', views.PictureView.as_view())
]