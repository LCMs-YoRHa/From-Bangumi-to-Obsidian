<!-- src/components/icons/IconRestore.vue -->
<template>
  <svg
    xmlns="http://www.w3.org/2000/svg"
    :width="size"
    :height="size"
    viewBox="0 0 24 24"
    fill="none"
    stroke="currentColor"
    stroke-width="2"
    stroke-linecap="round"
    stroke-linejoin="round"
  >
    <rect x="5" y="7" width="14" height="14" rx="2" ry="2" />
    <path d="M9 3h10a2 2 0 0 1 2 2v10" />
  </svg>
</template>

<script setup lang="ts">
withDefaults(defineProps<{ size?: number | string }>(), { size: 16 });
</script>
