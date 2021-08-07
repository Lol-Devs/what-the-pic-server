<template>
  <div>

    <router-link :to="'/cam/'+id">
    
      <div class="mx-10 my-2 p-3 hover:bg-gray-200 cursor-pointer border-2 grid grid-cols-2">

<p class="ml-20">Camera No. {{id}}</p>

<div v-if="status==0" class="flex items-center md:mr-48 mr-10">
<div class="h-3 w-3 rounded-full bg-green-600 mr-3" ></div>
<span>
  online
</span>
</div>

<div v-if="status==1" class="flex items-center md:mr-48 mr-10">
<div class="h-3 w-3 rounded-full bg-red-600 mr-3" ></div>
<span>
  offline
</span>
</div>

<div v-if="status==2" class="flex items-center md:mr-48 mr-10">
<div class="h-3 w-3 rounded-full bg-blue-600 mr-3" ></div>
<span>
  deactivated
</span>
</div>

<div v-if="status==3" class="flex items-center md:mr-48 mr-10">
<div class="h-3 w-3 rounded-full bg-red-600 mr-3 animate-pulse" ></div>
<span>
  error
</span>
</div>

<div v-if="status==4" class="flex items-center md:mr-48 mr-10">
<div class="h-3 w-3 rounded-full bg-yellow-600 mr-3" ></div>
<span>
  sd card missing
</span>
</div>

</div>

    </router-link>
    
    

  </div>
</template>

<script>
export default {
  name: "Test",
  created() {},
  data() {
    return {
      id: this.id,
      status: this.status
    };
  },
  props: {
    id: Number,
    status: Number,
  },
  methods: {},
};
</script>

<style lang="scss" scoped></style>