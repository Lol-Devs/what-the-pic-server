# Generated by Django 3.2.6 on 2021-08-07 09:07

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('cameras', '0007_alter_picture_picture'),
    ]

    operations = [
        migrations.AddField(
            model_name='camera',
            name='quality',
            field=models.PositiveIntegerField(default=10),
        ),
    ]
